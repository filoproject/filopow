# FILOPOW fair launch

FILOPOW's only reason to exist is to bring a mining community into the FILO
ecosystem. The thing being earned with a fair launch is that community's
**trust**. This document is the public commitment; it is meant to be checkable,
not just stated.

## The commitments

- **No premine.** No coins exist before the public start. The genesis coinbase
  is unspendable by Bitcoin-lineage design.
- **No developer fee, no founder allocation, no set-aside.** 100% of every block
  reward is mined by the public. There is no block-reward split, no tax address,
  no "community fund" that pays the team.
- **Announced start.** The mining start time (T0, UTC) is published in advance.
  The full source and the release binaries are published *before* T0, so anyone
  can build and be ready at the same moment.
- **No head start for anyone, including us.** The genesis block is timestamped at
  T0 with a launch-week news headline. Nobody can produce valid blocks before
  that timestamp — not the operators, not insiders. The operators may not mine
  at all.

## How to verify these claims

- **No premine / dev fee:** read `src/validation.cpp` `GetBlockSubsidy()` — the
  reward is a single value paid entirely to the block's miner, with no second
  output. Read `src/chainparams.cpp` — the genesis coinbase pays a standard
  script and is unspendable.
- **Announced timing:** the `pszTimestamp` in the genesis (visible in
  `getblock` of block 0 and in the source) contains a headline from launch week,
  which did not exist before then.
- **Binaries match source:** every release is built by public CI from a tagged
  commit, and each file ships with a `.sha256sum`. Build it yourself and compare.
- **Supply:** `MAX_MONEY` in `src/amount.h` is ~21,000,000 FPOW; the emission
  schedule is in [filopow-params.md](filopow-params.md).

## Security policy (published up front)

FILOPOW is a small proof-of-work chain at launch. Consensus stays vanilla — no
masternodes, no ChainLocks (see the fork's design notes). That leaves rental
51% / deep-reorg risk, which is handled operationally, transparently:

- **Exchanges and the FILO bridge set high deposit confirmations** on FILOPOW,
  adjustable, and can pause deposits per-direction if the chain misbehaves.
- **Emergency checkpoints:** if a deep, damaging reorg occurs, the operators may
  publish a checkpoint to pin the honest chain. Any such action will be
  announced openly, with the block height and hash and the reason, before or as
  it happens — never silently. A checkpoint is a last resort, not a routine
  tool.

Surprises are what kill trust in a small chain. The policy above is the whole
policy; there is nothing withheld.
