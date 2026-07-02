# Mining FILOPOW

FILOPOW uses **KawPow** — the same GPU proof-of-work as Ravencoin and Neoxa. Any
miner and pool that already supports KawPow works with FILOPOW; you only change
the coin/address/pool endpoint.

FILOPOW is KawPow from the first block. There is **no CPU mining** — it is a GPU
chain by design.

> Fair launch: do not point a rig at FILOPOW before the announced start time.
> The genesis is timestamped at T0 and code + binaries are published ahead of
> it, so nobody — including the operators — can mine early. See
> [fair-launch.md](fair-launch.md).

## What a miner earns

The base subsidy is **5 FPOW per block** (halving every 2,100,000 blocks). The
miner's share of it depends on the network's phase:

| Phase | Miner share |
|---|---|
| Before 150 smartnodes are registered | ~90% (dev fee is the only deduction) |
| After 150 smartnodes, dev-fee period (blocks ≤ 1,050,000) | 36% |
| After 150 smartnodes, post-sunset | 40% |

**The earliest miners earn the most** — until the smartnode tier fills up
(150 nodes × 5,000 FPOW collateral), their share goes to the miner. Transaction
fees follow the miner/smartnode split.

## 1. Get a wallet address

Run a node (see the release binaries / README) and create an address:

```
filopow-cli getnewaddress
```

Mainnet addresses start with `F`. That address receives your mining rewards.

## 2. GPU miners

Point them at a pool's stratum endpoint (recommended) or at your own node via a
stratum proxy for solo. Replace `<pool-host>:<stratum-port>` with the pool from
the launch announcement and `<FPOW-address>` with your own `F...` address.

### kawpowminer (open source, reference)

```
kawpowminer -P stratum+tcp://<FPOW-address>.<worker>@<pool-host>:<stratum-port>
```

### T-Rex (NVIDIA)

```
t-rex -a kawpow -o stratum+tcp://<pool-host>:<stratum-port> \
      -u <FPOW-address> -p x -w <worker>
```

### GMiner (NVIDIA/AMD)

```
miner --algo kawpow --server <pool-host>:<stratum-port> \
      --user <FPOW-address>.<worker>
```

### TeamRedMiner / lolMiner (AMD)

```
teamredminer -a kawpow -o stratum+tcp://<pool-host>:<stratum-port> \
             -u <FPOW-address>.<worker> -p x

lolMiner --algo KAWPOW --pool <pool-host>:<stratum-port> \
         --user <FPOW-address>.<worker>
```

## 3. HiveOS

FILOPOW is a KawPow coin: create a **custom KawPow coin** (or copy an RVN/NEOX
flight sheet and swap wallet + pool):

- Coin: `FPOW` (custom) — algo `kawpow`
- Wallet: your `F...` address
- Pool: the announced pool's `host:stratum-port`
- Miner: T-Rex / GMiner / TeamRedMiner / lolMiner as above

## 4. Solo mining (your own node)

The node exposes `getblocktemplate` over RPC (port 7766). KawPow miners speak
stratum, so solo mining needs a stratum bridge in front of `getblocktemplate`.

```
filopow.conf:
  server=1
  rpcuser=<user>
  rpcpassword=<strong-password>
  rpcallowip=127.0.0.1
```

Run a KawPow-capable solo stratum proxy against `127.0.0.1:7766` and point the
GPU miner at the proxy. A fresh chain starts at minimum difficulty, so early
blocks are findable by a single modern GPU.

## 5. Confirmations, ChainLocks

FILOPOW inherits Neoxa's **ChainLocks**: once enough smartnodes are online and
the operator enables the chainlock sporks, quorums of smartnodes sign the chain
tip and deep reorgs become impossible — a much stronger guarantee than
confirmation counts alone. Until ChainLocks are active (early network life),
exchanges and the FILO bridge use high confirmation counts instead; expect
requirements to relax once ChainLocks are enforced.
