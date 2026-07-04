// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2020 The Neoxa developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include "arith_uint256.h"

#include <assert.h>

#include "chainparamsseeds.h"
static size_t lastCheckMnCount = 0;
static int lastCheckHeight= 0;
static bool lastCheckedLowLLMQParams = false;

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << CScriptNum(0) << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nNonce64   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    // FILOPOW launch is 07 Jul 2026 (mainnet genesis nTime). For the strongest
    // no-premine proof, swap this line for a real 07-Jul-2026 news headline and
    // regrind all four geneses on the day (keep nKawPowActivationTime =
    // primitives/block.h = mainnet genesis nTime + 1); the date and all params
    // are otherwise final.
    const char* pszTimestamp = "FILOPOW genesis 07 Jul 2026 - fair launch, no premine, GPU + smartnodes for FILO";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nWindowSize, int64_t nThreshold)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
    if (nWindowSize != -1) {
            consensus.vDeployments[d].nWindowSize = nWindowSize;
    }
    if (nThreshold != -1) {
            consensus.vDeployments[d].nThreshold = nThreshold;
    }
}

//void CChainParams::UpdateDIP3Parameters(int nActivationHeight, int nEnforcementHeight)
//{
//    consensus.DIP0003Height = nActivationHeight;
//    consensus.DIP0003EnforcementHeight = nEnforcementHeight;
//}

void CChainParams::UpdateBudgetParameters(int nSmartnodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    consensus.nSmartnodePaymentsStartBlock = nSmartnodePaymentsStartBlock;
    consensus.nBudgetPaymentsStartBlock = nBudgetPaymentsStartBlock;
    consensus.nSuperblockStartBlock = nSuperblockStartBlock;
}

void CChainParams::UpdateSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    consensus.nMinimumDifficultyBlocks = nMinimumDifficultyBlocks;
    consensus.nHighSubsidyBlocks = nHighSubsidyBlocks;
    consensus.nHighSubsidyFactor = nHighSubsidyFactor;
}

void CChainParams::UpdateLLMQChainLocks(Consensus::LLMQType llmqType) {
    consensus.llmqTypeChainLocks = llmqType;
}

static void FindMainNetGenesisBlock(uint32_t nTime, uint32_t nBits, const char* network)
{

    CBlock block = CreateGenesisBlock(nTime, 0, nBits, 4, 5 * COIN);

    arith_uint256 bnTarget;
    bnTarget.SetCompact(block.nBits);
    block.nNonce64 = -1;
    for (uint32_t nNonce = 0; nNonce < UINT32_MAX; nNonce++) {
        block.nNonce = nNonce;
        block.nNonce64++;
        uint256 mix_hash;
        uint256 hash = block.GetHashFull(mix_hash);
        if (nNonce % 48 == 0) {
        	printf("\nrnonce=%d, pow is %s\n", nNonce, hash.GetHex().c_str());
        }
        if (UintToArith256(hash) <= bnTarget) {
            block.mix_hash = mix_hash;
        	printf("\n%s net\n", network);
        	printf("\ngenesis is %s\n", block.ToString().c_str());
			printf("\npow is %s\n", hash.GetHex().c_str());
			printf("\ngenesisNonce is %d\n", nNonce);
			std::cout << "Genesis Merkle " << block.hashMerkleRoot.GetHex() << std::endl;
        	return;
        }

    }

    // This is very unlikely to happen as we start the devnet with a very low difficulty. In many cases even the first
    // iteration of the above loop will give a result already
    error("%sNetGenesisBlock: could not find %s genesis block",network, network);
    assert(false);
}
static Consensus::LLMQParams llmq200_2 = {
        .type = Consensus::LLMQ_50_60,
        .name = "llmq_3_200",
        .size = 200,
        .minSize = 2,
        .threshold = 2,

        .dkgInterval = 30, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
        .dkgBadVotesThreshold = 8,

        .signingActiveQuorumCount = 2, // just a few ones to allow easier testing

        .keepOldConnections = 3,
};

static Consensus::LLMQParams llmq3_60 = {
        .type = Consensus::LLMQ_50_60,
        .name = "llmq_3_60",
        .size = 3,
        .minSize = 2,
        .threshold = 2,

        .dkgInterval = 30, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
        .dkgBadVotesThreshold = 8,

        .signingActiveQuorumCount = 2, // just a few ones to allow easier testing

        .keepOldConnections = 3,
};

static Consensus::LLMQParams llmq5_60 = {
        .type = Consensus::LLMQ_400_60,
        .name = "llmq_20_60",
        .size = 5,
        .minSize = 4,
        .threshold = 3,

        .dkgInterval = 30 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 28,
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // two days worth of LLMQs

        .keepOldConnections = 5,
};

static Consensus::LLMQParams llmq5_85 = {
        .type = Consensus::LLMQ_400_85,
        .name = "llmq_20_85",
        .size = 5,
        .minSize = 4,
        .threshold = 3,

        .dkgInterval = 30 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 48, // give it a larger mining window to make sure it is mined
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // four days worth of LLMQs

        .keepOldConnections = 5,
};

static Consensus::LLMQParams llmq20_60 = {
        .type = Consensus::LLMQ_400_60,
        .name = "llmq_20_60",
        .size = 20,
        .minSize = 15,
        .threshold = 12,

        .dkgInterval = 30 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 28,
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // two days worth of LLMQs

        .keepOldConnections = 5,
};

static Consensus::LLMQParams llmq20_85 = {
        .type = Consensus::LLMQ_400_85,
        .name = "llmq_20_85",
        .size = 20,
        .minSize = 18,
        .threshold = 17,

        .dkgInterval = 30 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 48, // give it a larger mining window to make sure it is mined
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // four days worth of LLMQs

        .keepOldConnections = 5,
};

static Consensus::LLMQParams llmq10_60 = {
        .type = Consensus::LLMQ_50_60,
        .name = "llmq_10_60",
        .size = 10,
        .minSize = 8,
        .threshold = 7,

        .dkgInterval = 30, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
        .dkgBadVotesThreshold = 8,

        .signingActiveQuorumCount = 6, // just a few ones to allow easier testing

        .keepOldConnections = 7,
};

static Consensus::LLMQParams llmq40_60 = {
        .type = Consensus::LLMQ_400_60,
        .name = "llmq_40_60",
        .size = 40,
        .minSize = 30,
        .threshold = 24,

        .dkgInterval = 30 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 28,
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // two days worth of LLMQs

        .keepOldConnections = 5,
};

static Consensus::LLMQParams llmq40_85 = {
        .type = Consensus::LLMQ_400_85,
        .name = "llmq_40_85",
        .size = 40,
        .minSize = 35,
        .threshold = 34,

        .dkgInterval = 30 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 48, // give it a larger mining window to make sure it is mined
        .dkgBadVotesThreshold = 30,

        .signingActiveQuorumCount = 4, // four days worth of LLMQs

        .keepOldConnections = 5,
};

static Consensus::LLMQParams llmq50_60 = {
        .type = Consensus::LLMQ_50_60,
        .name = "llmq_50_60",
        .size = 50,
        .minSize = 40,
        .threshold = 30,

        .dkgInterval = 30, // one DKG per hour
        .dkgPhaseBlocks = 2,
        .dkgMiningWindowStart = 10, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 18,
        .dkgBadVotesThreshold = 40,

        .signingActiveQuorumCount = 24, // a full day worth of LLMQs

        .keepOldConnections = 25,
};

static Consensus::LLMQParams llmq400_60 = {
        .type = Consensus::LLMQ_400_60,
        .name = "llmq_400_60",
        .size = 400,
        .minSize = 300,
        .threshold = 240,

        .dkgInterval = 30 * 12, // one DKG every 12 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 28,
        .dkgBadVotesThreshold = 300,

        .signingActiveQuorumCount = 4, // two days worth of LLMQs

        .keepOldConnections = 5,
};

// Used for deployment and min-proto-version signalling, so it needs a higher threshold
static Consensus::LLMQParams llmq400_85 = {
        .type = Consensus::LLMQ_400_85,
        .name = "llmq_400_85",
        .size = 400,
        .minSize = 350,
        .threshold = 340,

        .dkgInterval = 30 * 24, // one DKG every 24 hours
        .dkgPhaseBlocks = 4,
        .dkgMiningWindowStart = 20, // dkgPhaseBlocks * 5 = after finalization
        .dkgMiningWindowEnd = 48, // give it a larger mining window to make sure it is mined
        .dkgBadVotesThreshold = 300,

        .signingActiveQuorumCount = 4, // four days worth of LLMQs

        .keepOldConnections = 5,
};

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 2100000; // Note: actual number of blocks per calendar year with DGW v3 is ~200700 (for example 449750 - 249050)
        consensus.nSmartnodePaymentsStartBlock = 1000; // legacy pre-DIP3 field; real gates are DIP0003Height + the 10-smartnode count
        consensus.nSmartnodePaymentsIncreaseBlock = 2000; // legacy, unused with deterministic smartnodes
        consensus.nSmartnodePaymentsIncreasePeriod = 576*30; // 17280 - actual historical value
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 24;
        consensus.nBudgetPaymentsStartBlock = INT_MAX; // actual historical value
        consensus.nBudgetPaymentsCycleBlocks = 16616; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nBudgetPaymentsWindowBlocks = 100;
        consensus.nSuperblockStartBlock = INT_MAX; // The block at which 12.1 goes live (end of final 12.0 budget cycle)
        consensus.nSuperblockStartHash = uint256S("0000000000020cb27c7ef164d21003d5d20cdca2f54dd9a9ca6d45f4d47f8aa3");
        consensus.nSuperblockCycle = 16616; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nGovernanceMinQuorum = 10;
        consensus.nGovernanceFilterElements = 20000;
        consensus.nSmartnodeMinimumConfirmations = 15;
        consensus.BIPCSVEnabled = true;
        consensus.BIP34Enabled = true;
        consensus.BIP65Enabled = true; // 00000000000076d8fcea02ec0963de4abfd01e771fec0863f960c2c64fe6f357
        consensus.BIP66Enabled = true; // 00000000000b1fa2dfa312863570e13fae9ca7b5566cb27e55422620b469aefa
        consensus.nSegwitEnabled = true;
        consensus.DIP0003Height = 1; // deterministic smartnodes from the start (fresh chain; devnet precedent)
        consensus.DIP0008Enabled = true;
       // consensus.DIP0003EnforcementHeight = 1047200;
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 2016 * 60; // Filopow: 1 day
        consensus.nPowTargetSpacing = 1 * 60; // Filopow: 1 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nPowDGWHeight = 1;
        consensus.DGWBlocksAvg = 60;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.smartnodePaymentFixedBlock = 1; // enforce smartnode payee from the start (soft until 10 smartnodes exist)
        consensus.nAssetsForkBlock = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1653004800; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1653264000; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].bit = 6;  //Assets (HIP2)
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nOverrideRuleChangeActivationThreshold = 1814;
        //consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].bit = 7;  // Assets (HIP5)
        consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nOverrideRuleChangeActivationThreshold = 1714; // Approx 85% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].bit = 8;
        consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nOverrideRuleChangeActivationThreshold = 1714; // Approx 85% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].bit = 9;
        consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nOverrideRuleChangeActivationThreshold = 1411; // Approx 70% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].bit = 10;
        consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nOverrideRuleChangeActivationThreshold = 1411; // Approx 70% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].bit = 3;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nStartTime = 1688961600;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nTimeout = 1720584000; 
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nThreshold = 3226; // 80% of 4032

        // Fresh chain: no accumulated work yet.
        consensus.nMinimumChainWork = uint256S("0x00");

        // Fresh chain: validate every signature from genesis.
        consensus.defaultAssumeValid = uint256S("0x00");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x46; // F
        pchMessageStart[1] = 0x50; // P
        pchMessageStart[2] = 0x4f; // O
        pchMessageStart[3] = 0x57; // W
        nDefaultPort = 7767;
        nPruneAfterHeight = 100000;
   //   FindMainNetGenesisBlock(1652138420, 0x20001fff, "main");
        uint32_t nGenesisTime = 1783429200; // 07 Jul 2026 13:00:00 UTC (= nKawPowActivationTime - 1)

	    genesis = CreateGenesisBlock(nGenesisTime, 22286728, 0x1e00ffff, 4, 5 * COIN);
        consensus.hashGenesisBlock = genesis.GetX16RHash();
	    assert(consensus.hashGenesisBlock == uint256S("0000007efd4b4e86228279d20fc796c793d54898ebcb02a98cd1991f2bc4d24a"));
        assert(genesis.hashMerkleRoot == uint256S("57d79c44b12e7c5d1f8e2afb7e469b9b23bf43344dcd652051d00b0212591730"));

        // Fresh chain: no DNS seeds yet. Populate with our own seed nodes before
        // the launch announcement (Phase 5).


        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,36);  // 'F'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,95);  // 'f'
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        // FILOPOW BIP44 cointype in mainnet is '7767' (matches the default P2P port)
        nExtCoinType = 7767;

        // FILOPOW dev fee: a transparent, disclosed 10% of the block subsidy that
        // SUNSETS: 10% for blocks up to 1,050,000 (~2 years at 1-min blocks),
        // 0% forever after. It funds filodex/exchange liquidity. There is no
        // premine and no insider allocation. The address below is a burn-style
        // PLACEHOLDER with no known key — REPLACE with the real operator
        // treasury address before the mainnet launch (like the genesis regrind).
        vector<FounderRewardStructure> rewardStructures = { {1050000, 10}, // 10% dev fee until ~2yr
                                                            {INT_MAX, 0}  // sunset: 0% forever after
                                                            };
        consensus.nFounderPayment = FounderPayment(rewardStructures, 1, "Fs388X5W9h9Ung1qd2qCxpCiZMAJZ185nr"); // mainnet dev-fee treasury (WIF at /root/validator-keys/filopow)
        consensus.nSpecialRewardShare = Consensus::SpecialRewardShare(0.6,0.4,0.0);
        // Smartnode collateral 5,000 FPOW. Smartnode share of (subsidy+fees):
        // 54% while the dev fee runs, 60% after the sunset — miner keeps the
        // remainder (36% -> 40%). Paid only once >=10 smartnodes are registered
        // (GetSmartnodePayment); until then the miner receives the whole share.
        consensus.nCollaterals = SmartnodeCollaterals(
          {
            {INT_MAX, 5000 * COIN}
          },
          { {1050000, 54}, {INT_MAX, 60} }
        );

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        //vFixedSeeds = std::vector<SeedSpec6>();
        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq3_60;
        consensus.llmqs[Consensus::LLMQ_400_60] = llmq20_60;
        consensus.llmqs[Consensus::LLMQ_400_85] = llmq20_85;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_400_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;
        miningRequiresPeers = true;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;
        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour

        vSporkAddresses = {"Fs8XNV3sz4xLJXQn3BUJ3MgAeWpu6A2cML"}; // FILOPOW operator spork key (WIF at /root/validator-keys/filopow, hold offline)
        nMinSporkKeys = 1;
        fBIP9CheckSmartnodesUpgraded = true;

        checkpointData = {
            {
                {0, uint256S("0000007efd4b4e86228279d20fc796c793d54898ebcb02a98cd1991f2bc4d24a")},
            }
	    };

        chainTxData = ChainTxData{
            // Fresh chain: no transaction history yet.
            0,
            0,
            0
        };

         // Burn Amounts — Raven-family fee schedule scaled 1/1000, matching the
        // 1/1000-scaled emission (5 FPOW/block, ~21M max supply). Integer math.
        nIssueAssetBurnAmount = COIN / 2;                // 0.5 FPOW
        nReissueAssetBurnAmount = COIN / 10;             // 0.1 FPOW
        nIssueSubAssetBurnAmount = COIN / 10;            // 0.1 FPOW
        nIssueUniqueAssetBurnAmount = COIN / 200;        // 0.005 FPOW
        nIssueMsgChannelAssetBurnAmount = COIN / 10;     // 0.1 FPOW
        nIssueQualifierAssetBurnAmount = 1 * COIN;       // 1 FPOW
        nIssueSubQualifierAssetBurnAmount = COIN / 10;   // 0.1 FPOW
        nIssueRestrictedAssetBurnAmount = 3 * COIN / 2;  // 1.5 FPOW
        nAddNullQualifierTagBurnAmount = COIN / 10000;   // 0.0001 FPOW

        // Burn Addresses (prefix byte 36 -> 'F'; fixed X-pattern payloads with a
        // recomputed checksum, no known keys)
	    strIssueAssetBurnAddress = "FXissueAssetXXXXXXXXXXXXXXXXXUQiaG";
        strReissueAssetBurnAddress = "FXReissueAssetXXXXXXXXXXXXXXTXRFtA";
        strIssueSubAssetBurnAddress = "FXissueSubAssetXXXXXXXXXXXXXW8XgnQ";
        strIssueUniqueAssetBurnAddress = "FXissueUniqueAssetXXXXXXXXXXVLfask";
        strIssueMsgChannelAssetBurnAddress = "FXissueMsgChanneLAssetXXXXXXSPXmAr";
        strIssueQualifierAssetBurnAddress = "FXissueQuaLifierXXXXXXXXXXXXW6Sw85";
        strIssueSubQualifierAssetBurnAddress = "FXissueSubQuaLifierXXXXXXXXXX5h64b";
        strIssueRestrictedAssetBurnAddress = "FXissueRestrictedXXXXXXXXXXXWoghPk";
        strAddNullQualifierTagBurnAddress = "FXaddTagBurnXXXXXXXXXXXXXXXXUz54UP";
        //Global Burn Address
        strGlobalBurnAddress = "FXBurnXXXXXXXXXXXXXXXXXXXXXXTSYoiG";
        // No gaming layer on FILOPOW: keep this legacy field pointed at the
        // global burn address (it is only referenced by IsBurnAddress()).
        strCommunityAutonomousAddress = "FXBurnXXXXXXXXXXXXXXXXXXXXXXTSYoiG";

        nAssetActivationHeight = 1; // Asset activated block height
        nMessagingActivationBlock = 1; // Messaging activated block height
        nRestrictedActivationBlock = 1; // Restricted activated block height
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nSmartnodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nSmartnodePaymentsIncreaseBlock
        consensus.nSmartnodePaymentsIncreaseBlock = 4030;
        consensus.nSmartnodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = INT_MAX;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = INT_MAX; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on testnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on testnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nSmartnodeMinimumConfirmations = 1;
        consensus.BIP34Enabled = true;
        consensus.BIP65Enabled = true; // 0000039cf01242c7f921dcb4806a5994bc003b48c1973ae0c89b67809c2bb2ab
        consensus.BIP66Enabled = true; // 0000002acdd29a14583540cb72e1c5cc83783560e38fa7081495d474fe1671f7
        consensus.nSegwitEnabled = true;
        consensus.DIP0003Height = 2;
        consensus.DIP0008Enabled = true;
        consensus.BIPCSVEnabled = true;
     //   consensus.DIP0003EnforcementHeight = 7300;
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 2016 * 60; // Filopow: 1 day
        consensus.nPowTargetSpacing = 60; // Filopow: 1 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nPowDGWHeight = 1;
		consensus.DGWBlocksAvg = 60;
        consensus.smartnodePaymentFixedBlock = 1;
        consensus.nAssetsForkBlock = 1;
        consensus.nRuleChangeActivationThreshold = 1613; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].bit = 6;  //Assets (HIP2)
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nOverrideRuleChangeActivationThreshold = 1814;
        //consensus.vDeployments[Consensus::DEPLOYMENT_ASSETS].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].bit = 7;  // Assets (HIP5)
        consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nOverrideRuleChangeActivationThreshold = 1714; // Approx 85% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_MSG_REST_ASSETS].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].bit = 8;
        consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nOverrideRuleChangeActivationThreshold = 1714; // Approx 85% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_TRANSFER_SCRIPT_SIZE].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].bit = 9;
        consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nOverrideRuleChangeActivationThreshold = 1411; // Approx 70% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_ENFORCE_VALUE].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].bit = 10;
        consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nStartTime = 1653004800; // Friday, 20 May 2022 00:00:00
        consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nTimeout = 1653264000; // Monday, 23 May 2022 00:00:00
        //consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nOverrideRuleChangeActivationThreshold = 1411; // Approx 70% of 2016
        //consensus.vDeployments[Consensus::DEPLOYMENT_COINBASE_ASSETS].nOverrideMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].bit = 3;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nStartTime = 1688961600;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nTimeout = 1693213889; 
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nWindowSize = 4032;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0003].nThreshold = 3226; // 80% of 4032

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0"); // 0

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0"); // 0

        pchMessageStart[0] = 0x66; // f
        pchMessageStart[1] = 0x70; // p
        pchMessageStart[2] = 0x6f; // o
        pchMessageStart[3] = 0x77; // w
        nDefaultPort = 17767;
        nPruneAfterHeight = 1000;
        
        uint32_t nGenesisTime = 1783006613; // X16R genesis (< nKawPowActivationTime), every mined block is KawPow

        genesis = CreateGenesisBlock(nGenesisTime, 78, 0x20001fff, 4, 5 * COIN);
        consensus.hashGenesisBlock = genesis.GetX16RHash();
        assert(consensus.hashGenesisBlock == uint256S("0011712b79c259d5e0639016ed7a50f93cd4a4def726241b3623e88b50677d91"));
        assert(genesis.hashMerkleRoot == uint256S("57d79c44b12e7c5d1f8e2afb7e469b9b23bf43344dcd652051d00b0212591730"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // Fresh chain: no testnet DNS seeds yet — add before launch (Phase 5).

        // Testnet FILOPOW addresses start with 'm' or 'n' (Bitcoin testnet defaults)
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        // Testnet FILOPOW script addresses start with '2' (Bitcoin testnet defaults)
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet Filopow BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        // Testnet Filopow BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        // Testnet FILOPOW BIP44 coin type is '1' (all coins' testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq3_60;
		consensus.llmqs[Consensus::LLMQ_400_60] = llmq20_60;
		consensus.llmqs[Consensus::LLMQ_400_85] = llmq20_85;
		consensus.llmqTypeChainLocks = Consensus::LLMQ_400_60;
		consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;

		// Same economics as mainnet, with the dev-fee sunset at height 2000 so it
		// is actually exercisable on testnet.
		consensus.nCollaterals = SmartnodeCollaterals(
			{
				{INT_MAX, 5000 * COIN}
			},
			{
				{2000, 54}, {INT_MAX, 60}
			}
		);

        vector<FounderRewardStructure> rewardStructures = {  {2000, 10}, // 10% dev fee until the testnet sunset height
                                                             {INT_MAX, 0} // sunset
                                                										   };
		consensus.nFounderPayment = FounderPayment(rewardStructures, 1, "mm5TiSGhnHRi8u7j7G8GVavSvQLzz2j3ew");
        consensus.nSpecialRewardShare = Consensus::SpecialRewardShare(0.6,0.4,0.0);

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fRequireRoutableExternalIP = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = true;
        miningRequiresPeers = true;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"n3dvEANDLbcekEev8yaFRw9Bjky5caN3V8"}; // FILOPOW testnet spork key
        nMinSporkKeys = 1;
        fBIP9CheckSmartnodesUpgraded = true;

        checkpointData = (CCheckpointData) {
            {

            }
        };

        chainTxData = ChainTxData{
        	// Update as we know more about the contents of the Filopow chain
            //1658331968, // * UNIX timestamp of last known number of transactions
            //4108,     // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
           // 0.01518114964117619        // * estimated number of transactions per second after that timestamp
        };

        /** FPOW Start **/
        // Burn Amounts — Raven-family fee schedule scaled 1/1000 (see mainnet)
        nIssueAssetBurnAmount = COIN / 2;                // 0.5 FPOW
        nReissueAssetBurnAmount = COIN / 10;             // 0.1 FPOW
        nIssueSubAssetBurnAmount = COIN / 10;            // 0.1 FPOW
        nIssueUniqueAssetBurnAmount = COIN / 200;        // 0.005 FPOW
        nIssueMsgChannelAssetBurnAmount = COIN / 10;     // 0.1 FPOW
        nIssueQualifierAssetBurnAmount = 1 * COIN;       // 1 FPOW
        nIssueSubQualifierAssetBurnAmount = COIN / 10;   // 0.1 FPOW
        nIssueRestrictedAssetBurnAmount = 3 * COIN / 2;  // 1.5 FPOW
        nAddNullQualifierTagBurnAmount = COIN / 10000;   // 0.0001 FPOW

        // Burn Addresses (standard testnet prefix byte 111)
	    strIssueAssetBurnAddress = "n1issueAssetXXXXXXXXXXXXXXXXWdnemQ";
        strReissueAssetBurnAddress = "n1ReissueAssetXXXXXXXXXXXXXXWG9NLd";
        strIssueSubAssetBurnAddress = "n1issueSubAssetXXXXXXXXXXXXXbNiH6v";
        strIssueUniqueAssetBurnAddress = "n1issueUniqueAssetXXXXXXXXXXS4695i";
        strIssueMsgChannelAssetBurnAddress = "n1issueMsgChanneLAssetXXXXXXT2PBdD";
        strIssueQualifierAssetBurnAddress = "n1issueQuaLifierXXXXXXXXXXXXUysLTj";
        strIssueSubQualifierAssetBurnAddress = "n1issueSubQuaLifierXXXXXXXXXYffPLh";
        strIssueRestrictedAssetBurnAddress = "n1issueRestrictedXXXXXXXXXXXXZVT9V";
        strAddNullQualifierTagBurnAddress = "n1addTagBurnXXXXXXXXXXXXXXXXX5oLMH";
	    //Global Burn Address
        strGlobalBurnAddress = "n1BurnXXXXXXXXXXXXXXXXXXXXXXU1qejP";

	    // No gaming layer on FILOPOW (see mainnet)
        strCommunityAutonomousAddress = "n1BurnXXXXXXXXXXXXXXXXXXXXXXU1qejP";

        nAssetActivationHeight = 1; // Asset activated block height
        nMessagingActivationBlock = 1; // Messaging activated block height
        nRestrictedActivationBlock = 1; // Restricted activated block height
        /** FILOPOW End **/
    }
};

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams() {
        strNetworkID = "dev";
        consensus.nSubsidyHalvingInterval = 210240;
        consensus.nSmartnodePaymentsStartBlock = 4010; // not true, but it's ok as long as it's less then nSmartnodePaymentsIncreaseBlock
        consensus.nSmartnodePaymentsIncreaseBlock = 4030;
        consensus.nSmartnodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 4100;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 4200; // NOTE: Should satisfy nSuperblockStartBlock > nBudgetPeymentsStartBlock
        consensus.nSuperblockStartHash = uint256(); // do not check this on devnet
        consensus.nSuperblockCycle = 24; // Superblocks can be issued hourly on devnet
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nSmartnodeMinimumConfirmations = 1;
        consensus.BIPCSVEnabled = true;
        consensus.BIP34Enabled = true; // BIP34 activated immediately on devnet
        consensus.BIP65Enabled = true; // BIP65 activated immediately on devnet
        consensus.BIP66Enabled = true; // BIP66 activated immediately on devnet
        consensus.DIP0003Height = 1; // DIP0003 activated immediately on devnet
        consensus.DIP0008Enabled = true;
       // consensus.DIP0003EnforcementHeight = 2; // DIP0003 activated immediately on devnet
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Filopow: 1 day
        consensus.nPowTargetSpacing = 2.5 * 60; // Filopow: 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowDGWHeight = 60;
		consensus.DGWBlocksAvg = 60;
        consensus.nAssetsForkBlock = 1;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        pchMessageStart[0] = 0xe2;
        pchMessageStart[1] = 0xca;
        pchMessageStart[2] = 0xff;
        pchMessageStart[3] = 0xce;
        nDefaultPort = 19799;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1783006413, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x411583b2164126a2dd83e968511ab64cf5b6777249a9a8d34e89a4c49f5b9264"));
        assert(genesis.hashMerkleRoot == uint256S("0x4291076213861926a81625dee79b49ac2968dd005b6d9d1b18bf2e2bf8e25d50"));

        vector<FounderRewardStructure> rewardStructures = {  {INT_MAX, 5}// devnet: flat 5% for payment-path testing
                                                                										   };
		consensus.nFounderPayment = FounderPayment(rewardStructures, 200, "yaackz5YDLnFuuX6gGzEs9EMRQGfqmNYjc");
        consensus.nSpecialRewardShare = Consensus::SpecialRewardShare(0.8,0.2,0.0);

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("filopowevo.org",  "devnet-seed.filopowevo.org"));

        // Testnet Filopow addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
        // Testnet Filopow script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Testnet Filopow BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        // Testnet Filopow BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        // Testnet Filopow BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
        consensus.llmqs[Consensus::LLMQ_400_60] = llmq400_60;
        consensus.llmqs[Consensus::LLMQ_400_85] = llmq400_85;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_50_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        vSporkAddresses = {"yjPtiKh2uwk3bDutTEA2q9mCtXyiZRWn55"};
        nMinSporkKeys = 1;
        // devnets are started with no blocks and no MN, so we can't check for upgraded MN (as there are none)
        fBIP9CheckSmartnodesUpgraded = false;

        checkpointData = (CCheckpointData) {
            {
                { 0, uint256S("0x411583b2164126a2dd83e968511ab64cf5b6777249a9a8d34e89a4c49f5b9264")},
            }
        };

        chainTxData = ChainTxData{

        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nSmartnodePaymentsStartBlock = 240;
        consensus.nSmartnodePaymentsIncreaseBlock = 350;
        consensus.nSmartnodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 2;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = INT_MAX;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = INT_MAX;
        consensus.nSuperblockStartHash = uint256(); // do not check this on regtest
        consensus.nSuperblockCycle = 10;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;
        consensus.nSmartnodeMinimumConfirmations = 1;
        consensus.BIPCSVEnabled = true;
        consensus.BIP34Enabled = true; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP65Enabled = true; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Enabled = true; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.DIP0003Height = 432;
        consensus.DIP0008Enabled = true;
       // consensus.DIP0003EnforcementHeight = 500;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 24 * 60 * 60; // Filopow: 1 day
        consensus.nPowTargetSpacing = 2 * 60; // Filopow: 2.5 minutes
        consensus.nMinimumDifficultyBlocks = 2000;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nPowDGWHeight = 60;
		consensus.DGWBlocksAvg = 60;
        consensus.nAssetsForkBlock = 1;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        consensus.nCollaterals = SmartnodeCollaterals(
          {   {INT_MAX, 10 * COIN}  },
          {  {240, 0}, {INT_MAX, 20} });

        pchMessageStart[0] = 0xfc;
        pchMessageStart[1] = 0xc1;
        pchMessageStart[2] = 0xb7;
        pchMessageStart[3] = 0xdc;
        nDefaultPort = 19899;
        nPruneAfterHeight = 1000;
        
        //FindMainNetGenesisBlock(1417713337, 0x20001fff, "regtest");

        genesis = CreateGenesisBlock(1783006513, 2395, 0x20001fff, 4, 5 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x001dde5b96caa2eb455144104e1886b35365cb7e5eae7e4432afefd16af86afd"));
        assert(genesis.hashMerkleRoot == uint256S("0x57d79c44b12e7c5d1f8e2afb7e469b9b23bf43344dcd652051d00b0212591730"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fRequireRoutableExternalIP = false;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes
        nPoolMinParticipants = 3;
        nPoolMaxParticipants = 5;

        // privKey: cP4EKFyJsHT39LDqgdcB43Y3YXjNyjb5Fuas1GQSeAtjnZWmZEQK
        vSporkAddresses = {"yj949n1UH6fDhw6HtVE5VMj2iSTaSWBMcW"};
        nMinSporkKeys = 1;
        // regtest usually has no smartnodes in most tests, so don't check for upgraged MNs
        fBIP9CheckSmartnodesUpgraded = false;
        vector<FounderRewardStructure> rewardStructures = { {4000, 15},// 15% founder/dev fee
                                                            {INT_MAX, 10}// 10% founder/dev fee forever
                                                     										   };
        consensus.nFounderPayment = FounderPayment(rewardStructures, 500, "yaackz5YDLnFuuX6gGzEs9EMRQGfqmNYjc");
        consensus.nSpecialRewardShare = Consensus::SpecialRewardShare(0.8,0.2,0.0);

        checkpointData = (CCheckpointData) {
            {
                // (upstream Neoxa mistakenly pointed this at the devnet genesis)
                {0, uint256S("0x001dde5b96caa2eb455144104e1886b35365cb7e5eae7e4432afefd16af86afd")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        // Regtest Filopow addresses start with 'y'
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,140);
        // Regtest Filopow script addresses start with '8' or '9'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,19);
        // Regtest private keys start with '9' or 'c' (Bitcoin defaults)
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        // Regtest Filopow BIP32 pubkeys start with 'tpub' (Bitcoin defaults)
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        // Regtest Filopow BIP32 prvkeys start with 'tprv' (Bitcoin defaults)
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        // Regtest Filopow BIP44 coin type is '1' (All coin's testnet default)
        nExtCoinType = 1;

        // long living quorum params
        consensus.llmqs[Consensus::LLMQ_50_60] = llmq3_60;
        consensus.llmqs[Consensus::LLMQ_400_60] = llmq20_60;
        consensus.llmqs[Consensus::LLMQ_400_85] = llmq20_85;
        consensus.llmqTypeChainLocks = Consensus::LLMQ_400_60;
        consensus.llmqTypeInstantSend = Consensus::LLMQ_50_60;
    }
};

static std::unique_ptr<CChainParams> globalChainParams;

const CChainParams &Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::DEVNET) {
        return std::unique_ptr<CChainParams>(new CDevNetParams());
    } else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout, int64_t nWindowSize, int64_t nThreshold)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout, nWindowSize, nThreshold);
}

//void UpdateDIP3Parameters(int nActivationHeight, int nEnforcementHeight)
//{
//    globalChainParams->UpdateDIP3Parameters(nActivationHeight, nEnforcementHeight);
//}

void UpdateBudgetParameters(int nSmartnodePaymentsStartBlock, int nBudgetPaymentsStartBlock, int nSuperblockStartBlock)
{
    globalChainParams->UpdateBudgetParameters(nSmartnodePaymentsStartBlock, nBudgetPaymentsStartBlock, nSuperblockStartBlock);
}

void UpdateDevnetSubsidyAndDiffParams(int nMinimumDifficultyBlocks, int nHighSubsidyBlocks, int nHighSubsidyFactor)
{
    globalChainParams->UpdateSubsidyAndDiffParams(nMinimumDifficultyBlocks, nHighSubsidyBlocks, nHighSubsidyFactor);
}

void UpdateDevnetLLMQChainLocks(Consensus::LLMQType llmqType)
{
    globalChainParams->UpdateLLMQChainLocks(llmqType);
}

void UpdateLLMQParams(size_t totalMnCount, int height, bool lowLLMQParams) {
	globalChainParams->UpdateLLMQParams(totalMnCount, height, lowLLMQParams);
}
bool IsMiningPhase(Consensus::LLMQParams params, int nHeight)
{
    int phaseIndex = nHeight % params.dkgInterval;
    if (phaseIndex >= params.dkgMiningWindowStart && phaseIndex <= params.dkgMiningWindowEnd) {
        return true;
    }
    return false;
}

bool IsLLMQsMiningPhase(int nHeight) {
	for(auto& it : globalChainParams->GetConsensus().llmqs) {
		if(IsMiningPhase(it.second, nHeight)) {
			return true;
		}
	}
	return false;
}

void CChainParams::UpdateLLMQParams(size_t totalMnCount, int height, bool lowLLMQParams) {
	bool isNotLLMQsMiningPhase;
    //on startup if block height % dkgInterval fall between dkgMiningWindowStart and dkgMiningWindowEnd
    //it will not switch to the correct llmq. if lastCheckHeight is 0 then force switch to the correct llmq
    if(lastCheckHeight < height &&
    		(lastCheckMnCount != totalMnCount || lastCheckedLowLLMQParams != lowLLMQParams) &&
			((isNotLLMQsMiningPhase = !IsLLMQsMiningPhase(height)) || lastCheckHeight == 0)) {
	    LogPrintf("---UpdateLLMQParams %d-%d-%ld-%ld-%d\n", lastCheckHeight, height, lastCheckMnCount, totalMnCount, isNotLLMQsMiningPhase);
		lastCheckMnCount = totalMnCount;
		lastCheckedLowLLMQParams = lowLLMQParams;
		lastCheckHeight = height;
		if(totalMnCount < 5) {
			consensus.llmqs[Consensus::LLMQ_50_60] = llmq3_60;
			if(strcmp(Params().NetworkIDString().c_str(),"test") == 0) {
				consensus.llmqs[Consensus::LLMQ_400_60] = llmq5_60;
				consensus.llmqs[Consensus::LLMQ_400_85] = llmq5_85;
			} else {
				consensus.llmqs[Consensus::LLMQ_400_60] = llmq20_60;
				consensus.llmqs[Consensus::LLMQ_400_85] = llmq20_85;
			}
		} else if(totalMnCount < 100) {
			consensus.llmqs[Consensus::LLMQ_50_60] = llmq10_60;
			consensus.llmqs[Consensus::LLMQ_400_60] = llmq20_60;
			consensus.llmqs[Consensus::LLMQ_400_85] = llmq20_85;
		}  else if(totalMnCount < 600) {
			consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
			consensus.llmqs[Consensus::LLMQ_400_60] = llmq40_60;
			consensus.llmqs[Consensus::LLMQ_400_85] = llmq40_85;
		} else {
			consensus.llmqs[Consensus::LLMQ_50_60] = llmq50_60;
			consensus.llmqs[Consensus::LLMQ_400_60] = llmq400_60;
			consensus.llmqs[Consensus::LLMQ_400_85] = llmq400_85;
		}
        if(lowLLMQParams){
            consensus.llmqs[Consensus::LLMQ_50_60] = llmq200_2;
        }		
	}
}
