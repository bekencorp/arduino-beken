# FAQ

## Is this only an Arduino Core?

No. The repository also contains the staging, packaging, and SDK export logic required to make the Arduino-facing layer usable in Arduino CLI and Arduino IDE workflows.

## Which targets are currently supported?

The public target set is currently:

- `bk7239n`

## Do I need to run `prepare-platform` manually every time?

Not always. High-level targets such as `cli-compile`, `build`, `export-sdk`, and release-oriented targets already chain through the required generation flow. You still need `prepare-platform` when you explicitly want to inspect or refresh the staged platform.

## Which build path should I use first?

For the first success path, use Arduino CLI validation with a shipped example. It exercises the staged platform the same way most users will consume it.

## Are the generic variants board definitions?

They are platform baselines, not exhaustive descriptions of every vendor board. If your board wiring diverges materially, treat the generic variant as a starting point rather than a final board definition.

## Why are some examples defensive at runtime?

Some features depend on the exact staged SDK payload linked into the image. A runtime guard is sometimes better than making the whole example fail to build when the rest of the platform is otherwise usable.
