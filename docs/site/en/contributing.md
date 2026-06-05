# Contributing

## What Fits This Repository Well

Good contributions usually fall into one of these categories:

- Arduino Core feature work
- variant and target improvements
- example additions and fixes
- staging, packaging, and export tooling improvements
- documentation that shortens the path from checkout to successful validation

## Keep the Contribution Boundary Clear

When making a change, be explicit about which layer you are touching:

- Arduino-facing source and APIs
- variant mappings and board assumptions
- packaging and staging flow
- exported SDK behavior
- user-facing documentation

This makes review much easier.

## Prefer Example-Backed Changes

If a change affects a user-visible feature, add or update an example whenever practical. This repository already relies on examples as both documentation and validation anchors.

## Prefer Small, Verifiable Changes

Changes are easier to review when they:

- isolate one problem
- avoid unrelated cleanup
- state the target SoC impact clearly
- mention the validation flow that was run

## Validation Expectations

At minimum, contributors should say how they validated the change:

- `prepare-platform`
- `cli-compile`
- direct `bk_idk` build
- hardware verification, if relevant

## Documentation Changes Count

Improving the entry path, clarifying target assumptions, and documenting edge cases are all valuable contributions here, especially when they remove ambiguity from bring-up or packaging workflows.
