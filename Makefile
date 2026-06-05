.PHONY: all help prepare-platform sync-core-wrappers check-core-wrappers prepare-sketch build build-base cli-compile cli-upload export-sdk release release-check docs-install docs-html docs-clean clean

SDK_DIR ?=
TARGET ?= bk7239n
VARIANT ?=
SKETCH ?= $(CURDIR)/libraries/Blink/examples/Blink
PYTHON ?= python3
BASE_URL ?=
DOCS_VENV ?= $(PROJECT_ROOT)/.venv-docs
DOCS_PYTHON ?= $(DOCS_VENV)/bin/python
DOCS_PIP ?= $(DOCS_VENV)/bin/pip
DOCS_SPHINX_BUILD ?= $(DOCS_VENV)/bin/sphinx-build

PROJECT_ROOT := $(abspath .)
PROJECT_NAME := $(notdir $(PROJECT_ROOT))
VERSION := $(strip $(shell tr -d '\r\n' < "$(PROJECT_ROOT)/VERSION"))

BUILD_DIR ?= $(PROJECT_ROOT)/build
DIST_DIR ?= $(PROJECT_ROOT)/dist
SDK_EXPORT_DIR ?= $(DIST_DIR)/sdk/$(TARGET)
RELEASE_DIR ?= $(DIST_DIR)/release/$(TARGET)
RELEASE_CHECK_DIR ?= $(DIST_DIR)/release-check/$(TARGET)
PLATFORM_STAGE_DIR ?= $(BUILD_DIR)/platform/$(TARGET)/arduino-beken
CLI_PLATFORM_STAGE_DIR ?= $(BUILD_DIR)/platform-cli/$(TARGET)/arduino-beken
CLI_HARDWARE_LINK ?= $(PROJECT_ROOT)/.arduino-cli-test/user/hardware/beken/$(TARGET)

UPLOADER_ROOT ?= $(PROJECT_ROOT)/tools/bk_loader
UPLOADER_LINUX_ROOT ?= $(UPLOADER_ROOT)/linux
UPLOADER_WIN_ROOT ?= $(UPLOADER_ROOT)/windows
TOOLCHAIN_MANIFEST ?= $(PROJECT_ROOT)/tools/toolchains.json
TOOLCHAIN_LINUX_ROOT ?=
TOOLCHAIN_LINUX_URL ?=
TOOLCHAIN_LINUX_ARCHIVE_NAME ?=
TOOLCHAIN_LINUX_CHECKSUM ?=
TOOLCHAIN_LINUX_SIZE ?=
TOOLCHAIN_WIN_ROOT ?=
TOOLCHAIN_WIN_URL ?=
TOOLCHAIN_WIN_ARCHIVE_NAME ?=
TOOLCHAIN_WIN_CHECKSUM ?=
TOOLCHAIN_WIN_SIZE ?=
TOOLCHAIN_MACOS_X64_ROOT ?=
TOOLCHAIN_MACOS_X64_URL ?=
TOOLCHAIN_MACOS_X64_ARCHIVE_NAME ?=
TOOLCHAIN_MACOS_X64_CHECKSUM ?=
TOOLCHAIN_MACOS_X64_SIZE ?=
TOOLCHAIN_MACOS_ARM64_ROOT ?=
TOOLCHAIN_MACOS_ARM64_URL ?=
TOOLCHAIN_MACOS_ARM64_ARCHIVE_NAME ?=
TOOLCHAIN_MACOS_ARM64_CHECKSUM ?=
TOOLCHAIN_MACOS_ARM64_SIZE ?=

CLI_CONFIG ?= $(PROJECT_ROOT)/.arduino-cli.yaml
CLI_SKETCH ?= $(PROJECT_ROOT)/libraries/Blink/examples/Blink
CLI_FQBN ?= beken:$(TARGET):$(TARGET)_generic
CLI_BUILD_PATH ?= $(PROJECT_ROOT)/.arduino-cli-test/build-$(TARGET)-$(notdir $(basename $(CLI_SKETCH)))
CLI_COMPILER_PATH ?= /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/
CLI_PORT ?=
CLI_COMPILE_EXTRA_ARGS ?=
CLI_UPLOAD_EXTRA_ARGS ?=
ARDUINO_BEKEN_RELEASE_GZIP_LEVEL ?= 3
SDK_EXPORT_ARGS :=
ifneq ($(strip $(SDK_DIR)),)
SDK_EXPORT_ARGS := --sdk-dir "$(SDK_DIR)" --build-root "$(BUILD_DIR)"
endif

GENERATED_DIR := $(BUILD_DIR)/generated
GENERATED_SKETCH := $(GENERATED_DIR)/sketch.cpp
GENERATED_SKETCH_EXTRA_C := $(GENERATED_DIR)/sketch-extra.c
GENERATED_SKETCH_EXTRA_CPP := $(GENERATED_DIR)/sketch-extra.cpp
REQUESTED_GOALS := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),all)
SDK_REQUIRED_GOALS := all build build-base

ifneq ($(filter $(SDK_REQUIRED_GOALS),$(REQUESTED_GOALS)),)
ifeq ($(strip $(SDK_DIR)),)
$(error ERROR: SDK_DIR is not defined. Usage: make SDK_DIR=/path/to/bk_idk TARGET=bk7239n build)
endif
endif

all: build

help:
	@echo "arduino-beken build entry"
	@echo ""
	@echo "Targets:"
	@echo "  make SDK_DIR=/path/to/bk_idk TARGET=bk7239n build SKETCH=libraries/Blink/examples/Blink"
	@echo "  make TARGET=bk7239n cli-compile CLI_SKETCH=libraries/Blink/examples/Blink"
	@echo "  make TARGET=bk7239n cli-upload CLI_SKETCH=libraries/Blink/examples/Blink CLI_PORT=/dev/ttyUSB0"
	@echo "  make TARGET=bk7239n release-check"
	@echo "  make SDK_DIR=/path/to/bk_idk TARGET=bk7239n export-sdk"
	@echo "  make TARGET=bk7239n release BASE_URL=https://example.com/downloads"
	@echo "  make TARGET=bk7239n prepare-platform"
	@echo "  make docs-install"
	@echo "  make docs-html"
	@echo "  make sync-core-wrappers"
	@echo "  make TARGET=bk7239n check-core-wrappers"
	@echo ""
	@echo "Variables:"
	@echo "  SDK_DIR        Path to bk_idk; required for build/build-base/all, optional for export-sdk refresh"
	@echo "  TARGET         Target SoC, default bk7239n"
	@echo "  VARIANT        Optional variant directory name for make-driven builds"
	@echo "  SKETCH         Sketch file or sketch directory, default libraries/Blink/examples/Blink"
	@echo "  CLI_SKETCH     Sketch path for arduino-cli targets, default ./libraries/Blink/examples/Blink"
	@echo "  CLI_FQBN       FQBN for arduino-cli targets, default beken:\$$TARGET:\$$TARGET_generic"
	@echo "  CLI_BUILD_PATH Build path for arduino-cli targets, default ./.arduino-cli-test/build-\$$TARGET-<sketch>"
	@echo "  CLI_COMPILER_PATH ARM GCC bin dir for arduino-cli targets, default /opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin/"
	@echo "  CLI_PORT       Serial port for cli-upload, required there"
	@echo "  CLI_CONFIG     arduino-cli config file, default ./.arduino-cli.yaml"
	@echo "  BUILD_DIR      Build output root, default ./build"
	@echo "  PLATFORM_STAGE_DIR Staged Arduino platform root, default ./build/platform/\$$TARGET/arduino-beken"
	@echo "  CLI_PLATFORM_STAGE_DIR Local CLI-only platform root, default ./build/platform-cli/\$$TARGET/arduino-beken"
	@echo "  SDK_EXPORT_DIR Exported SDK root, default ./dist/sdk/\$$TARGET"
	@echo "  RELEASE_DIR    Release output root, default ./dist/release/\$$TARGET"
	@echo "  RELEASE_CHECK_DIR Lightweight release-check output, default ./dist/release-check/\$$TARGET"
	@echo "  BASE_URL       Public download URL prefix for Boards Manager artifacts"
	@echo "  ARDUINO_BEKEN_RELEASE_GZIP_LEVEL gzip level 0-9 for release .tar.gz (default 3; 1 fastest, 9 smallest)"
	@echo "  UPLOADER_ROOT  Checked-in bk_loader root, default ./tools/bk_loader"
	@echo "  UPLOADER_LINUX_ROOT  Linux bk_loader dir for release packaging, default \$$UPLOADER_ROOT/linux"
	@echo "  UPLOADER_WIN_ROOT    Windows bk_loader dir for release packaging, default \$$UPLOADER_ROOT/windows"
	@echo "  TOOLCHAIN_MANIFEST Toolchain metadata manifest, default ./tools/toolchains.json"
	@echo "  TOOLCHAIN_LINUX_ROOT Optional Linux ARM GCC root override"
	@echo "  TOOLCHAIN_LINUX_URL  Optional Linux ARM GCC archive URL override"
	@echo "  TOOLCHAIN_WIN_ROOT   Optional Windows ARM GCC root override"
	@echo "  TOOLCHAIN_WIN_URL    Optional Windows ARM GCC archive URL override"
	@echo "  TOOLCHAIN_MACOS_X64_URL Optional macOS x86_64 ARM GCC archive URL override"
	@echo "  TOOLCHAIN_MACOS_ARM64_URL Optional macOS arm64 ARM GCC archive URL override"

prepare-platform:
	@$(PYTHON) "$(PROJECT_ROOT)/tools/stage_platform.py" \
		--project-root "$(PROJECT_ROOT)" \
		--target "$(TARGET)" \
		--version "$(VERSION)" \
		--output-dir "$(PLATFORM_STAGE_DIR)" \
		--cli-output-dir "$(CLI_PLATFORM_STAGE_DIR)" \
		--cli-link "$(CLI_HARDWARE_LINK)"

sync-core-wrappers:
	@$(PYTHON) "$(PROJECT_ROOT)/tools/generate_core_wrappers.py" \
		--project-root "$(PROJECT_ROOT)"


check-core-wrappers: prepare-platform
	@$(PYTHON) "$(PROJECT_ROOT)/tools/check_core_wrappers.py" \
		--project-root "$(PROJECT_ROOT)" \
		--core-dir "$(PLATFORM_STAGE_DIR)/cores/arduino"

prepare-sketch:
	@mkdir -p "$(GENERATED_DIR)"
	@$(PYTHON) "$(PROJECT_ROOT)/tools/sketch_prep.py" \
		--sketch "$(SKETCH)" \
		--output "$(GENERATED_SKETCH)" \
		--extra-c-output "$(GENERATED_SKETCH_EXTRA_C)" \
		--extra-cpp-output "$(GENERATED_SKETCH_EXTRA_CPP)"

build: prepare-platform prepare-sketch
	@echo "========================================"
	@echo "Project Dir : $(PROJECT_ROOT)"
	@echo "SDK Dir     : $(SDK_DIR)"
	@echo "Target      : $(TARGET)"
	@if [ -n "$(VARIANT)" ]; then echo "Variant     : $(VARIANT)"; fi
	@echo "Sketch      : $(SKETCH)"
	@echo "Build Dir   : $(BUILD_DIR)"
	@echo "========================================"
	@ARMINO_SKETCH_CPP="$(GENERATED_SKETCH)" ARDUINO_VARIANT="$(VARIANT)" $(MAKE) -C "$(SDK_DIR)" "$(TARGET)" \
		PROJECT_DIR="$(PROJECT_ROOT)" \
		BUILD_DIR="$(BUILD_DIR)"

# Export the platform SDK from a sketch-free baseline build so Arduino CLI
# can provide its own sketch and core objects later.
build-base: prepare-platform
	@echo "========================================"
	@echo "Project Dir : $(PROJECT_ROOT)"
	@echo "SDK Dir     : $(SDK_DIR)"
	@echo "Target      : $(TARGET)"
	@if [ -n "$(VARIANT)" ]; then echo "Variant     : $(VARIANT)"; fi
	@echo "Build Dir   : $(BUILD_DIR)"
	@echo "========================================"
	@ARDUINO_VARIANT="$(VARIANT)" $(MAKE) -C "$(SDK_DIR)" "$(TARGET)" \
		PROJECT_DIR="$(PROJECT_ROOT)" \
		BUILD_DIR="$(BUILD_DIR)"

cli-compile: export-sdk
	@echo "========================================"
	@echo "CLI Config   : $(CLI_CONFIG)"
	@echo "Target       : $(TARGET)"
	@echo "FQBN         : $(CLI_FQBN)"
	@echo "Sketch       : $(CLI_SKETCH)"
	@echo "Build Path   : $(CLI_BUILD_PATH)"
	@echo "SDK Path     : $(SDK_EXPORT_DIR)"
	@echo "Compiler Path: $(CLI_COMPILER_PATH)"
	@echo "========================================"
	@arduino-cli compile --clean \
		--config-file "$(CLI_CONFIG)" \
		--fqbn "$(CLI_FQBN)" \
		--build-path "$(CLI_BUILD_PATH)" \
		--build-property "compiler.sdk.path=$(SDK_EXPORT_DIR)" \
		--build-property "compiler.path=$(CLI_COMPILER_PATH)" \
		$(CLI_COMPILE_EXTRA_ARGS) \
		"$(CLI_SKETCH)"

cli-upload: cli-compile
ifeq ($(strip $(CLI_PORT)),)
	$(error ERROR: CLI_PORT is required. Usage: make TARGET=bk7239n cli-upload CLI_SKETCH=libraries/Blink/examples/Blink CLI_PORT=/dev/ttyUSB0)
endif
	@echo "========================================"
	@echo "CLI Config   : $(CLI_CONFIG)"
	@echo "Target       : $(TARGET)"
	@echo "FQBN         : $(CLI_FQBN)"
	@echo "Sketch       : $(CLI_SKETCH)"
	@echo "Build Path   : $(CLI_BUILD_PATH)"
	@echo "Port         : $(CLI_PORT)"
	@echo "========================================"
	@arduino-cli upload \
		--config-file "$(CLI_CONFIG)" \
		--fqbn "$(CLI_FQBN)" \
		--build-path "$(CLI_BUILD_PATH)" \
		--port "$(CLI_PORT)" \
		$(CLI_UPLOAD_EXTRA_ARGS) \
		"$(CLI_SKETCH)"

export-sdk: prepare-platform
ifneq ($(strip $(SDK_DIR)),)
export-sdk: build-base
endif
	@mkdir -p "$(dir $(SDK_EXPORT_DIR))"
	@$(PYTHON) "$(PROJECT_ROOT)/tools/export_sdk.py" \
		--project-root "$(PROJECT_ROOT)" \
		--soc "$(TARGET)" \
		--version "$(VERSION)" \
		--output "$(SDK_EXPORT_DIR)" $(SDK_EXPORT_ARGS)

release: export-sdk
ifeq ($(strip $(BASE_URL)),)
	$(error ERROR: BASE_URL is required. Usage: make release BASE_URL=https://example.com/downloads)
endif
	@mkdir -p "$(RELEASE_DIR)"
	@ARDUINO_BEKEN_RELEASE_GZIP_LEVEL="$(ARDUINO_BEKEN_RELEASE_GZIP_LEVEL)" $(PYTHON) "$(PROJECT_ROOT)/tools/make_release.py" \
		--platform-root "$(PLATFORM_STAGE_DIR)" \
		--sdk-root "$(SDK_EXPORT_DIR)" \
		--target "$(TARGET)" \
		--version "$(VERSION)" \
		--base-url "$(BASE_URL)" \
		--output-dir "$(RELEASE_DIR)" \
		--toolchain-manifest "$(TOOLCHAIN_MANIFEST)" \
		--toolchain-linux-root "$(TOOLCHAIN_LINUX_ROOT)" \
		--toolchain-linux-url "$(TOOLCHAIN_LINUX_URL)" \
		--toolchain-linux-archive-name "$(TOOLCHAIN_LINUX_ARCHIVE_NAME)" \
		--toolchain-linux-checksum "$(TOOLCHAIN_LINUX_CHECKSUM)" \
		--toolchain-linux-size "$(TOOLCHAIN_LINUX_SIZE)" \
		--toolchain-win-root "$(TOOLCHAIN_WIN_ROOT)" \
		--toolchain-win-url "$(TOOLCHAIN_WIN_URL)" \
		--toolchain-win-archive-name "$(TOOLCHAIN_WIN_ARCHIVE_NAME)" \
		--toolchain-win-checksum "$(TOOLCHAIN_WIN_CHECKSUM)" \
		--toolchain-win-size "$(TOOLCHAIN_WIN_SIZE)" \
		--toolchain-macos-x64-root "$(TOOLCHAIN_MACOS_X64_ROOT)" \
		--toolchain-macos-x64-url "$(TOOLCHAIN_MACOS_X64_URL)" \
		--toolchain-macos-x64-archive-name "$(TOOLCHAIN_MACOS_X64_ARCHIVE_NAME)" \
		--toolchain-macos-x64-checksum "$(TOOLCHAIN_MACOS_X64_CHECKSUM)" \
		--toolchain-macos-x64-size "$(TOOLCHAIN_MACOS_X64_SIZE)" \
		--toolchain-macos-arm64-root "$(TOOLCHAIN_MACOS_ARM64_ROOT)" \
		--toolchain-macos-arm64-url "$(TOOLCHAIN_MACOS_ARM64_URL)" \
		--toolchain-macos-arm64-archive-name "$(TOOLCHAIN_MACOS_ARM64_ARCHIVE_NAME)" \
		--toolchain-macos-arm64-checksum "$(TOOLCHAIN_MACOS_ARM64_CHECKSUM)" \
		--toolchain-macos-arm64-size "$(TOOLCHAIN_MACOS_ARM64_SIZE)"

release-check: export-sdk
	@mkdir -p "$(RELEASE_CHECK_DIR)"
	@ARDUINO_BEKEN_RELEASE_GZIP_LEVEL="$(ARDUINO_BEKEN_RELEASE_GZIP_LEVEL)" $(PYTHON) "$(PROJECT_ROOT)/tools/make_release.py" \
		--platform-root "$(PLATFORM_STAGE_DIR)" \
		--sdk-root "$(SDK_EXPORT_DIR)" \
		--target "$(TARGET)" \
		--version "$(VERSION)" \
		--base-url "https://example.invalid/arduino-beken/$(TARGET)" \
		--output-dir "$(RELEASE_CHECK_DIR)" \
		--toolchain-manifest "$(TOOLCHAIN_MANIFEST)" \
		--toolchain-linux-root "$(TOOLCHAIN_LINUX_ROOT)" \
		--toolchain-linux-url "$(TOOLCHAIN_LINUX_URL)" \
		--toolchain-linux-archive-name "$(TOOLCHAIN_LINUX_ARCHIVE_NAME)" \
		--toolchain-linux-checksum "$(TOOLCHAIN_LINUX_CHECKSUM)" \
		--toolchain-linux-size "$(TOOLCHAIN_LINUX_SIZE)" \
		--toolchain-win-root "$(TOOLCHAIN_WIN_ROOT)" \
		--toolchain-win-url "$(TOOLCHAIN_WIN_URL)" \
		--toolchain-win-archive-name "$(TOOLCHAIN_WIN_ARCHIVE_NAME)" \
		--toolchain-win-checksum "$(TOOLCHAIN_WIN_CHECKSUM)" \
		--toolchain-win-size "$(TOOLCHAIN_WIN_SIZE)" \
		--toolchain-macos-x64-root "$(TOOLCHAIN_MACOS_X64_ROOT)" \
		--toolchain-macos-x64-url "$(TOOLCHAIN_MACOS_X64_URL)" \
		--toolchain-macos-x64-archive-name "$(TOOLCHAIN_MACOS_X64_ARCHIVE_NAME)" \
		--toolchain-macos-x64-checksum "$(TOOLCHAIN_MACOS_X64_CHECKSUM)" \
		--toolchain-macos-x64-size "$(TOOLCHAIN_MACOS_X64_SIZE)" \
		--toolchain-macos-arm64-root "$(TOOLCHAIN_MACOS_ARM64_ROOT)" \
		--toolchain-macos-arm64-url "$(TOOLCHAIN_MACOS_ARM64_URL)" \
		--toolchain-macos-arm64-archive-name "$(TOOLCHAIN_MACOS_ARM64_ARCHIVE_NAME)" \
		--toolchain-macos-arm64-checksum "$(TOOLCHAIN_MACOS_ARM64_CHECKSUM)" \
		--toolchain-macos-arm64-size "$(TOOLCHAIN_MACOS_ARM64_SIZE)" \
		--lightweight

docs-html:
	@rm -rf "$(PROJECT_ROOT)/build/docs/html"
	@DOCS_LANGUAGE=en "$(DOCS_SPHINX_BUILD)" -c "$(PROJECT_ROOT)/docs/site" -b html "$(PROJECT_ROOT)/docs/site" "$(PROJECT_ROOT)/build/docs/html"
	@DOCS_LANGUAGE=zh_CN "$(DOCS_SPHINX_BUILD)" -c "$(PROJECT_ROOT)/docs/site" -b html "$(PROJECT_ROOT)/docs/site/zh_CN" "$(PROJECT_ROOT)/build/docs/html/zh_CN"

docs-install:
	@$(PYTHON) -m venv "$(DOCS_VENV)"
	@"$(DOCS_PIP)" install -r "$(PROJECT_ROOT)/docs/site/requirements.txt"

docs-clean:
	rm -rf "$(PROJECT_ROOT)/build/docs"

clean:
	rm -rf "$(BUILD_DIR)"