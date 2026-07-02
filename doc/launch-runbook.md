# FILOPOW launch runbook

Operator checklist from "code builds and a node runs" to a live, fairly-launched
public chain. Do **all** of the pre-launch section before announcing a date, and
never mine before T0.

---

## Pre-launch (Phase 5)

### 1. Replace the two placeholders (consensus-critical)

1. **Treasury address.** `src/chainparams.cpp` mainnet `FounderPayment(...)`
   currently pays `FXTreasuryDevFundXXXXXXXXXXXTJfuCd` — a burn-style
   placeholder with **no key**. Replace it with the real operator treasury
   address (prefix `F`) whose key is held offline. If this is forgotten, the
   entire dev fee is burned.
2. **Genesis regrind.** Pick a real launch-week headline → `pszTimestamp` in
   `CreateGenesisBlock` (src/chainparams.cpp). Regrind all four networks'
   geneses (the session grinder does X16R = 16× blake512 for zero prev-hash;
   ~1 minute for all four) and update the asserts. Set
   `nKawPowActivationTime` (src/primitives/block.h) = new mainnet genesis
   nTime + 1. Keep testnet/devnet/regtest genesis times *below* the constant.

The **spork private keys** (mainnet/testnet) already exist and must be held
offline — they control quorum/ChainLocks/InstantSend activation.

### 2. Seed nodes (≥2, separate hosts)

Run public `filopowd` nodes with `-listen=1`, port 7767 reachable. Add each to
`contrib/seeds/nodes_main.txt` (`host:7767` lines), regenerate:
```
python3 contrib/seeds/generate-seeds.py contrib/seeds > src/chainparamsseeds.h
```
rebuild, commit. Optionally add a DNS seeder to `vSeeds` in `CMainParams`.
Document an `addnode=` list in the README as a fallback.

### 3. Explorer (eIquidus — same pattern as the other FILO coins)

Deploy against a `filopowd` with `txindex=1` (plus `addressindex=1`,
`assetindex=1`, `spentindex=1` for full explorer features). Publish the URL.

### 4. Mining access (both, before announcing)

- **Self-host a KawPow pool** against a synced node; publish the stratum
  endpoint in [mining.md](mining.md).
- Solo `getblocktemplate` + stratum-bridge instructions are already in
  [mining.md](mining.md).

### 5. Release binaries

Build linux + windows release binaries (the repo's Jenkins/ci scripts, or a
depends-based build), `sha256sum` every artifact, publish as a GitHub Release
on a tagged commit. Fair launch fails if only insiders have binaries.

### 6. Announcement (T-2 weeks minimum)

Publish with everything above live: start time (UTC), the reward table from
[filopow-params.md](filopow-params.md), the dev-fee disclosure + sunset from
[fair-launch.md](fair-launch.md), the treasury address, links (source, release,
explorer, pool, mining guide, smartnode guide). Bitcointalk ANN + Discord.

---

## Launch day (Phase 6)

1. **Before T0:** seed nodes synced to genesis; pool + explorer live; binaries
   public; announcement out.
2. **At T0:** miners produce block 1. Do **not** mine before T0 — any early
   block by anyone (including us) is exactly the betrayal the project forbids.
3. **First hours:** watch the first DGW difficulty epochs; be present in
   Discord. Deposit confirmations on the bridge stay high.

---

## Post-launch: smartnode + quorum activation sequence

The smartnode tier fills as collateral accumulates (5,000 FPOW each; payments
auto-activate at 150 registered). When enough smartnodes are online to form
quorums, the operator enables — in this order, each announced publicly:

1. `SPORK_17_QUORUM_DKG_ENABLED` — quorums begin forming (DKG).
2. Wait for stable quorum formation (`filopow-cli quorum list`).
3. `SPORK_19_CHAINLOCKS_ENABLED` — ChainLocks enforced; deep reorgs now
   rejected by consensus.
4. `SPORK_2_INSTANTSEND_ENABLED` (optional, after ChainLocks are stable).

Spork updates are signed with the operator spork key:
```
filopow-cli spork SPORK_17_QUORUM_DKG_ENABLED 0
```
(with `sporkkey=` loaded in the signing node's configuration).

After ChainLocks are enforced, bridge/exchange confirmation requirements can be
tuned down (announce that too).

---

## Phase 7 — filodex listing (separate filo-spot session)

Gateway UTXO family-clone pattern as with the other listed coins. FILOPOW keeps
the standard RPC surface (`getblockcount`, `getblockhash`, `getblock` verbosity
2, `getrawtransaction`, `sendrawtransaction`, `gettxout`, `validateaddress`,
`estimatesmartfee`, `-txindex=1`) and legacy P2PKH signing. Start with high
`min_confirmations`; relax after ChainLocks are enforced.
