# Troubleshooting

## Start with a Known Baseline

Before debugging a complex sketch, verify that all of the following are aligned:

- the expected `TARGET`
- the expected generic variant
- the staged hardware link for that target
- the exported SDK for the same target
- one of the shipped examples, ideally `examples/Blink`

## `arduino-cli` Cannot Find the Platform

Check whether:

- `make TARGET=<target> prepare-platform` was run
- `.arduino-cli-test/user/hardware/beken/<target>` points to the expected staged platform
- the `--fqbn` value matches the target and variant you intended to build

## Partition Scheme Selection Does Not Appear

Check whether:

- `config/<target>/` contains more than one `auto_partitions*.csv`
- the extra file name follows `auto_partitions_<name>.csv` or `auto_partitions.<name>.csv`
- `make TARGET=<target> prepare-platform` was rerun after adding the file

## The Build Fails Before Normal Sketch Compilation Starts

If the failure happens during the prebuild hook, first check:

- whether the selected `auto_partitions*.csv` exists
- whether the partition sizes satisfy upstream alignment rules
- whether the partition layout overlaps or exceeds flash capacity

## A Sketch Builds with `bk_idk` but Fails with `arduino-cli`

That usually means the issue is in one of these areas:

- staged platform content
- exported SDK payload
- Arduino CLI build properties
- sketch integration differences between the two flows

Reduce the problem to a shipped example before changing code.

## A Peripheral Example Compiles but Does Not Behave on Hardware

Check:

- whether the board actually exposes the pin
- whether that pin is already tied to another onboard function
- whether the peripheral needs external loopback or external wiring
- whether the board voltage and analog limits match the connected device

## Serial Output Is Missing

Confirm:

- the correct serial port
- `115200` baud, unless the sketch states otherwise
- that the board is actually rebooting into the newly flashed image

## What to Include in a Bug Report

Include:

- target SoC and variant
- host operating system
- the exact command you ran
- whether a shipped example reproduces the problem
- the shortest reproduction you can provide
