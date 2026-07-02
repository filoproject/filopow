# FILOPOW launch runbook (Phase 5–6)

Operator checklist to take FILOPOW from "code builds and a node runs" to a live,
fairly-launched public chain. Everything here is deployment the operator runs on
their own hosts; it needs no consensus changes.

Ordering matters: do **all** of Phase 5 before announcing a date, and never mine
before T0.

---

## Phase 5 — before the announced start

### 5.1 Regrind the genesis with a launch-week headline

The shipped genesis is provisional. Once the launch week is known:

1. Pick a real news headline from that week → set it as `pszTimestamp` in
   `src/chainparams.cpp` (`CreateGenesisBlock`).
2. Regrind mainnet, testnet, regtest genesis nonces (the genesis reduces to
   16× blake512 because `hashPrevBlock == 0`; a small grinder against
   `src/algo/blake.c` finds each in seconds — see the filo-spot session notes).
3. Keep `nKAWPOWActivationTime = <genesis nTime> + 1` for each network, so the
   genesis stays the only X16R block and every mined block is KawPow.
4. Update the three `assert(consensus.hashGenesisBlock == ...)` / merkle asserts.
5. Rebuild, confirm all three networks init (`getblockchaininfo` bestblockhash
   matches), commit.

### 5.2 Seed nodes (≥2, on separate hosts/networks)

1. Run `filopowd` on 2+ public hosts with `-listen=1`, port 7767 reachable.
2. Add each as a fixed seed and/or DNS seed:
   - Fixed seeds: put `host:7767` lines in `contrib/seeds/nodes_main.txt`, run
     `contrib/seeds/generate-seeds.py contrib/seeds > src/chainparamsseeds.h`,
     rebuild. (The arrays are empty until you do this.)
   - Optional DNS seed: run a DNS seeder and add it to `vSeeds` in
     `CMainParams`.
3. Document an `addnode=` list in the README as a fallback for early miners.

### 5.3 Block explorer (eIquidus)

Same pattern used for the other FILO coins:

1. Deploy eIquidus pointed at a `filopowd` with `-txindex=1`.
2. Configure coin=FILOPOW, ticker=FPOW, RPC on 7766, address prefix `F`.
3. Publish the explorer URL; it doubles as the HiveOS / wallet balance API and
   the third-party browser link the GUI can use (currently blank — see
   `DEFAULT_THIRD_PARTY_BROWSERS` in `src/qt/guiconstants.h`).

### 5.4 Mining access (both)

Fair launch fails if only insiders know how to point a rig.

1. **Self-host a KawPow pool** (any RVN-compatible pool stack, e.g. a KawPow
   fork of yiimp / a node-stratum-pool) against a `filopowd`. Publish the
   stratum `host:port`.
2. **Document solo mining** via `getblocktemplate` + a stratum bridge — already
   written in [mining.md](mining.md).
3. Make sure `doc/mining.md` names the real pool endpoint before launch.

### 5.5 Release binaries

1. Tag a release commit `vX.Y.Z` and push the tag — CI
   (`.github/workflows/build-filopow.yml`) builds linux/windows/arm/aarch64,
   checksums each artifact, and opens a **draft** GitHub Release with the files.
2. Review the draft, verify the `.sha256sum` files, publish it.
3. Link the release from the README and the announcement.

### 5.6 Announcement (T-2 weeks minimum)

Publish, with everything above already live:

- Start time (T0, UTC), from [fair-launch.md](fair-launch.md)
- Emission table + parameters, from [filopow-params.md](filopow-params.md)
- No-premine / no-dev-fee statement + how to verify it
- Links: source, release binaries + checksums, explorer, pool, mining guide
- Channels: Bitcointalk ANN (still the miner convention), Discord/website

---

## Phase 6 — launch day

1. **Before T0:** seed nodes up and synced to the genesis; pool and explorer
   running and pointed at a synced node; binaries public; announcement live.
2. **At T0:** the genesis timestamp is T0. Nodes accept the first mined blocks.
   Do **not** mine before this moment (any early block by anyone, including us,
   is exactly the betrayal the project forbids).
3. **First hours:** watch the first DGW epochs closely — a fresh chain starts at
   minimum difficulty and difficulty must respond as hashrate arrives. Be
   present in Discord.
4. **If a deep reorg happens:** follow the published emergency policy in
   [fair-launch.md](fair-launch.md) — announce openly before/as you act.

---

## Phase 7 — filodex listing (separate session)

Done in the filo-spot repo by the gateway UTXO family-clone pattern (same as the
other listed coins). FILOPOW's obligation to that work is only the RPC contract:
keep `getblockcount` / `getblockhash` / `getblock` (verbosity 2) /
`getrawtransaction` / `sendrawtransaction` / `gettxout` / `validateaddress` /
`estimatesmartfee` and `-txindex=1` working, and legacy P2PKH signing standard.
The bridge sets a high `min_confirmations` on the route at listing time.
