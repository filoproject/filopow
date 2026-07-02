# FILOPOW network parameters

Canonical reference for the FILOPOW chain. Every value here is set in
`src/chainparams.cpp` / `src/chainparamsbase.cpp` and is what the released
binaries enforce. This file is documentation; the source is authoritative.

## Identity

| | |
|---|---|
| Name | FILOPOW |
| Ticker | FPOW |
| Base | Ravencoin fork (KawPow) |
| Algorithm | KawPow (GPU), active from the first mined block |
| Address layer | Ravencoin asset layer, active from genesis |

## Emission

| | |
|---|---|
| Block time | 1 minute (target) |
| Block reward | 5 FPOW |
| Halving | every 2,100,000 blocks (~4 years) |
| Maximum supply | ~21,000,000 FPOW |
| Difficulty algorithm | DarkGravityWave, from block 1 |
| Premine / dev fee | none |

Emission is Ravencoin's curve scaled to 1/1000. The genesis coinbase is
unspendable by Bitcoin-lineage design and is part of the no-premine proof.

### Emission schedule (first halvings)

| Era | Blocks | Reward/block | Era supply | Cumulative |
|---|---|---|---|---|
| 0 | 0 – 2,099,999 | 5 FPOW | 10,500,000 | 10,500,000 |
| 1 | 2,100,000 – 4,199,999 | 2.5 FPOW | 5,250,000 | 15,750,000 |
| 2 | 4,200,000 – 6,299,999 | 1.25 FPOW | 2,625,000 | 18,375,000 |
| 3 | 6,300,000 – 8,399,999 | 0.625 FPOW | 1,312,500 | 19,687,500 |
| 4 | 8,400,000 – 10,499,999 | 0.3125 FPOW | 656,250 | 20,343,750 |
| … | … | halves each era | … | → ~21,000,000 |

## Network endpoints

| | Mainnet | Testnet | Regtest |
|---|---|---|---|
| P2P port | 7767 | 17767 | 18444 |
| RPC port | 7766 | 17766 | 18443 |
| Message magic (ASCII) | `FPOW` | `fpow` | `FPRT` |
| Data directory | `~/.filopow` | `~/.filopow/testnet1` | `~/.filopow/regtest` |
| Config file | `filopow.conf` | (same, `-testnet`) | (same, `-regtest`) |

BIP44 / SLIP-44 coin type (mainnet): **7767**.

## Address format (base58)

| Type | Mainnet byte | Leading char | Testnet byte | Leading char |
|---|---|---|---|---|
| P2PKH (pubkey) | 36 | `F` | 111 | `m` / `n` |
| P2SH (script) | 95 | `f` | 196 | `2` |
| WIF (secret key) | 128 | `5` / `K` / `L` | 239 | `9` / `c` |

There is no segwit / bech32 (Ravencoin lineage has none).

## Asset layer

The Ravencoin asset layer is active from genesis. Burn fees are Ravencoin's
schedule scaled 1/1000:

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
development. Before the launch announcement they are reground with a launch-week
news headline as `pszTimestamp` (public proof the chain could not have been
mined earlier). KawPow activates one second after the genesis timestamp, so the
genesis block itself is the only X16R-hashed block and every mined block is
KawPow from height 1.
