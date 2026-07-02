# FILOPOW network parameters

Canonical reference for the FILOPOW chain. Every value here is set in
`src/chainparams.cpp` / `src/chainparamsbase.cpp` and enforced by the released
binaries. This file is documentation; the source is authoritative.

## Identity

| | |
|---|---|
| Name | FILOPOW |
| Ticker | FPOW |
| Base | Neoxa fork (Ravencoin asset layer + Dash smartnode stack) |
| Mining | KawPow (GPU), active from the first mined block |
| Node services | Smartnodes: InstantSend, ChainLocks, deterministic list (DIP3) |

## Emission

| | |
|---|---|
| Block time | 1 minute (target, DGW from block 1) |
| Block reward | 5 FPOW |
| Halving | every 2,100,000 blocks (~4 years) |
| Maximum supply | ~21,000,000 FPOW |
| Premine | none |

## Block reward distribution

The block subsidy is split three ways, all consensus-enforced:

| Period | Miner | Smartnode | Development |
|---|---|---|---|
| Blocks 0 – 1,050,000 (~2 years) | 36% | 54% | 10% |
| After block 1,050,000 (sunset) | 40% | 60% | 0% — permanent |

- The **development fee** is paid to a disclosed treasury address in every
  block and funds market liquidity (the operators do not mine and hold no
  premine). It **ends permanently** at block 1,050,000 — enforced by consensus.
- The **smartnode share** activates automatically once **150 smartnodes** are
  registered; before that the miner receives it (early miners earn ~90% of the
  subsidy). Percentages are of (subsidy + fees) for smartnodes and of the
  subsidy for the development fee.
- Transaction fees follow the miner/smartnode split.

## Smartnodes

| | |
|---|---|
| Collateral | 5,000 FPOW (exact amount, single tier) |
| Registration | Dash-style deterministic list (DIP3 `protx`), active from height 1 |
| Payment activation | automatic at ≥150 registered smartnodes |
| ChainLocks / InstantSend | spork-gated (enabled by the operator once quorums can form) |

See [smartnodes.md](smartnodes.md) for setup.

## Network endpoints

| | Mainnet | Testnet | Regtest | Devnet |
|---|---|---|---|---|
| P2P port | 7767 | 17767 | 19899 | 19799 |
| RPC port | 7766 | 17766 | 19898 | 19798 |
| Message magic | `FPOW` | `fpow` | (Dash-lineage) | (Dash-lineage) |
| Data directory | `~/.filopowcore` | `.../testnet1` | `.../regtest` | `.../devnet` |
| Config file | `filopow.conf` | (same, `-testnet`) | (same, `-regtest`) | (same, `-devnet=`) |

BIP44 / SLIP-44 coin type (mainnet): **7767**.

## Address format (base58)

| Type | Mainnet byte | Leading char | Testnet byte | Leading char |
|---|---|---|---|---|
| P2PKH (pubkey) | 36 | `F` | 111 | `m` / `n` |
| P2SH (script) | 95 | `f` | 196 | `2` |
| WIF (secret key) | 128 | `5`/`K`/`L` | 239 | `9`/`c` |

Regtest/devnet keep Dash-lineage prefixes (140 `y` / 19 / 239) so the inherited
test fixtures stand. There is no segwit / bech32.

## Asset layer

Active from genesis. Burn fees are the Raven-family schedule scaled 1/1000:

| Operation | Burn fee (FPOW) |
|---|---|
| issue (root asset) | 0.5 |
| issue (sub-asset) | 0.1 |
| issue (unique) | 0.005 |
| reissue | 0.1 |
| issue (qualifier) | 1 |
| issue (sub-qualifier) | 0.1 |
| issue (restricted) | 1.5 |
| add / remove null-qualifier tag | 0.0001 (per address) |

## Genesis

The genesis blocks shipped in `src/chainparams.cpp` are **provisional** during
development. Before the launch announcement they are reground with a
launch-week news headline as `pszTimestamp` (public proof the chain could not
have been mined earlier). KawPow activates one second after the mainnet genesis
timestamp (`nKawPowActivationTime` in `src/primitives/block.h` — keep it
genesis+1 when regrinding); the genesis blocks are the only X16R-hashed blocks
and every mined block is KawPow.
