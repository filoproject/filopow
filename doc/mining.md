# Mining FILOPOW

FILOPOW uses **KawPow** — the same GPU proof-of-work Ravencoin uses. Any miner
and pool that already supports KawPow (RVN, Neoxa, Meowcoin, Clore) works with
FILOPOW; you only change the coin/address/pool endpoint.

FILOPOW is KawPow from the first block. There is **no CPU mining** — it is a
GPU chain by design.

> Fair launch: do not point a rig at FILOPOW before the announced start time.
> The genesis is timestamped at T0 and code + binaries are published ahead of
> it, so nobody — including the operators — can mine early. See
> [fair-launch.md](fair-launch.md).

## 1. Get a wallet address

Run a node (see the release binaries / README) and create an address:

```
filopow-cli getnewaddress
```

Mainnet addresses start with `F`. That address receives your mining rewards.

## 2. GPU miners

Popular KawPow miners and the flag that matters (`--algo kawpow` or the miner's
KawPow build). Point them at a pool's stratum endpoint (recommended) or at your
own node via a stratum proxy for solo.

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

Replace `<pool-host>:<stratum-port>` with the pool from the launch
announcement, and `<FPOW-address>` with your own `F...` address.

## 3. HiveOS

FILOPOW is a KawPow coin, so on HiveOS create a **custom KawPow coin** (or reuse
an RVN flight sheet and swap the wallet + pool):

- Coin: `FPOW` (custom) — algo `kawpow`
- Wallet: your `F...` address
- Pool: the announced pool's `host:stratum-port` (or a custom pool template)
- Miner: T-Rex / GMiner / TeamRedMiner / lolMiner as above

If you add FILOPOW as a fully custom coin, HiveOS also needs the coin's algo
(`kawpow`) and, for wallet-balance display, an explorer API URL (see the
explorer from the launch announcement).

## 4. Solo mining (your own node)

The node exposes `getblocktemplate` over RPC. KawPow miners speak stratum, so
solo mining needs a small **stratum bridge** in front of `getblocktemplate`
(e.g. a KawPow-capable solo stratum proxy). Point the proxy at your node's RPC:

```
filopow.conf:
  server=1
  rpcuser=<user>
  rpcpassword=<strong-password>
  rpcport=7766
  rpcallowip=127.0.0.1
```

Then run your stratum bridge against `127.0.0.1:7766` and point the GPU miner at
the bridge. Because a fresh chain starts at minimum difficulty, early blocks are
findable by a single modern GPU.

Raw template check (sanity, needs a peer so `getblocktemplate` is allowed):

```
filopow-cli getblocktemplate '{"rules":["segwit"]}'
```

The template's `version` is `0x30000000` (asset layer active), `coinbasevalue`
is `500000000` (5 FPOW), and `target` reflects the current difficulty.

## 5. Confirmations

FILOPOW keeps consensus vanilla (no ChainLocks / masternodes). Exchanges and the
FILO bridge set a **high confirmation count** on deposits to blunt rental-51%
risk at launch hashrate. If you are moving mined coins to an exchange or to
filodex, expect more confirmations than a large chain would ask for; this is a
deliberate, published safety policy, not a fault.
