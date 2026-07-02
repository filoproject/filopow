# Running a FILOPOW smartnode

A smartnode locks **5,000 FPOW** as collateral and provides InstantSend,
**ChainLocks**, and network services in exchange for the smartnode share of
every block reward (54% of subsidy+fees during the dev-fee period, 60% after
the sunset — see [filopow-params.md](filopow-params.md)).

FILOPOW uses the Dash-lineage **deterministic smartnode list** (DIP3): a
smartnode is created by an on-chain `protx` registration transaction and lives
until its collateral moves. There is no separate "start missing" dance.

## When payments start

Smartnode payments activate automatically once **150 smartnodes** are
registered network-wide. Before that, the smartnode share is paid to miners.
This is by design: at launch nobody holds collateral yet; as coins accumulate,
the tier fills and payments switch on by themselves. Your node can register at
any time — registration is valid from height 1.

## Requirements

- A VPS or server with a **static, routable IPv4**, reachable on port **7767**
- A synced `filopowd`
- Exactly **5,000 FPOW** in one UTXO (the collateral)
- A separate wallet for the collateral is strongly recommended

## Quick setup (Dash-style protx)

1. **Collateral.** Send exactly 5,000 FPOW to a fresh address in your wallet.
   Find its outpoint:
   ```
   filopow-cli smartnode outputs
   ```
2. **Keys.** Generate the operator BLS key on the smartnode host:
   ```
   filopow-cli bls generate
   ```
3. **Configure the smartnode host** (`filopow.conf`):
   ```
   smartnodeblsprivkey=<blsPrivKey from step 2>
   externalip=<your static IP>
   listen=1
   ```
4. **Register** from the wallet holding the collateral:
   ```
   filopow-cli protx register \
     <collateralHash> <collateralIndex> <ipAndPort> \
     <ownerKeyAddr> <operatorPubKey> <votingKeyAddr> \
     <operatorReward> <payoutAddress> (feeSourceAddress)
   ```
   - `ipAndPort`: `x.x.x.x:7767`
   - `ownerKeyAddr` / `votingKeyAddr`: fresh addresses you control
   - `operatorPubKey`: the BLS *public* key from step 2
   - `operatorReward`: 0 unless you split with a hosting operator
   - `payoutAddress`: where your smartnode rewards go
5. **Verify:**
   ```
   filopow-cli protx list valid 1
   filopow-cli smartnode status   # on the smartnode host
   ```

Keep the collateral unspent: moving it removes the smartnode from the list.

## ChainLocks and InstantSend

Quorum formation (DKG) and ChainLocks/InstantSend enforcement are switched on
by network **sporks** once the smartnode tier is large enough to form quorums.
The operators announce each activation. After ChainLocks are enforced, the
chain tip is signed by a smartnode quorum and deep reorgs are rejected by
consensus — this is the network's main defense against rental-hashrate attacks,
and it is what your smartnode is being paid to provide.
