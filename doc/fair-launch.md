# FILOPOW launch: the commitments

FILOPOW exists to bring a mining + smartnode community into the FILO ecosystem.
The thing being earned at launch is that community's **trust**. This document is
the public commitment; it is meant to be checkable, not just stated.

## The commitments

- **No premine.** No coins exist before the public start. The genesis coinbase
  is unspendable by Bitcoin-lineage design.
- **No insider allocation, no hidden mining.** The mining start time (T0, UTC)
  is published in advance. Full source and release binaries are published
  *before* T0, so anyone can build and be ready at the same moment. The genesis
  block is timestamped at T0 with a launch-week news headline — nobody,
  including the operators, can produce valid blocks before it.
- **One disclosed development fee — and it ends.** Blocks 0 through 1,050,000
  (~2 years) pay **10% of the block subsidy** to a single, published treasury
  address, in every block, visible on-chain. From block 1,050,001 the fee is
  **0% forever — enforced by consensus**, not by promise. That is the entire
  operator allocation; there is nothing else.

## Why a development fee at all

The operators do not premine and do not mine. Without any inventory, they could
not provide market liquidity on filodex or anywhere else — and a mineable coin
nobody can sell on day one is dead on arrival. The fee exists to fund that
liquidity and the network's infrastructure, in the open, with a hard end date.
Compare: our upstream (Neoxa) takes 15% falling to **10% forever**; FILOPOW
takes 10% falling to **zero**.

## How to verify each claim

- **No premine / the whole reward split:** `GetBlockSubsidy` and
  `GetSmartnodePayment` in `src/validation.cpp`, the founder structures in
  `src/chainparams.cpp` (`{1050000, 10}, {INT_MAX, 0}`), and
  `src/founder_payment.cpp`. The dev fee is a per-block coinbase output to the
  published address — watch it on the explorer.
- **The sunset:** consensus rejects any post-1,050,000 block that pays the dev
  fee address from the subsidy share (the required amount becomes 0; the miner
  keeps it). No new code path is needed to end it — it is in the reward table.
- **Announced timing:** the genesis `pszTimestamp` (visible in `getblock` of
  block 0 and in the source) contains a headline from launch week.
- **Binaries match source:** releases are built by public CI from a tagged
  commit; every file ships with a `.sha256sum`. Build it yourself and compare.
- **Supply:** `MAX_MONEY` in `src/amount.h` is ~21,000,000 FPOW; the emission
  schedule is in [filopow-params.md](filopow-params.md).

## Security: ChainLocks, and the policy until they're active

FILOPOW inherits Dash-style **ChainLocks** via its smartnode quorums: once
enough smartnodes are registered and the operator enables the quorum/chainlock
sporks, a quorum signs every chain tip and **51% reorg attacks stop working** —
an attacker with majority hashrate still cannot replace a locked chain.

Until the smartnode tier is large enough (early network life):

- Exchanges and the FILO bridge set **high deposit confirmations**, adjustable
  live, and can pause deposits per-direction if the chain misbehaves.
- **Emergency checkpoints** are a last resort: if a deep, damaging reorg occurs
  before ChainLocks are active, the operators may publish a checkpoint to pin
  the honest chain — announced openly with height, hash, and reason, never
  silently.
- The **spork keys** (network switches for ChainLocks/InstantSend and quorum
  formation) are held by the operators; every spork change is public on the
  network by construction and will be announced.

Surprises are what kill trust in a small chain. This page is the whole policy;
there is nothing withheld.
