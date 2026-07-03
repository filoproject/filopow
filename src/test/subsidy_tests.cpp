// Copyright (c) 2014-2018 The Dash Core developers
// Copyright (c) 2020 The Neoxa developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "validation.h"

#include "test/test_filopow.h"

#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(subsidy_tests, TestingSetup)

// FILOPOW subsidy is a flat 5 FPOW that halves every nSubsidyHalvingInterval
// (2,100,000) blocks and ignores the difficulty argument. (The upstream Dash
// test here checked a difficulty-dependent curve that FILOPOW does not use.)
BOOST_AUTO_TEST_CASE(block_subsidy_test)
{
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);
    const Consensus::Params& c = chainParams->GetConsensus();
    const CAmount base = 5 * COIN;

    // Era 0: every height below the first halving pays the full 5 FPOW.
    BOOST_CHECK_EQUAL(GetBlockSubsidy(0, 0, c, false), base);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(0, 1, c, false), base);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(0, c.nSubsidyHalvingInterval - 1, c, false), base);

    // The difficulty argument must not change the subsidy.
    BOOST_CHECK_EQUAL(GetBlockSubsidy(0x1c4a47c4, 4249, c, false), base);
    BOOST_CHECK_EQUAL(GetBlockSubsidy(0x1b10cf42, 99999, c, false), base);

    // Each halving epoch halves the reward.
    for (int halvings = 0; halvings < 20; ++halvings) {
        int nPrevHeight = halvings * c.nSubsidyHalvingInterval;
        CAmount expected = base >> halvings;
        BOOST_CHECK_EQUAL(GetBlockSubsidy(0, nPrevHeight, c, false), expected);
    }

    // Beyond 64 halvings the reward is zero.
    BOOST_CHECK_EQUAL(GetBlockSubsidy(0, 64 * c.nSubsidyHalvingInterval, c, false), 0);
}

// The summed emission stays under MAX_MONEY (~21,000,000 FPOW).
BOOST_AUTO_TEST_CASE(subsidy_limit_test)
{
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);
    const Consensus::Params& c = chainParams->GetConsensus();
    CAmount nSum = 0;
    for (int nHeight = 0; nHeight < 64 * c.nSubsidyHalvingInterval; nHeight += 1000) {
        CAmount nSubsidy = GetBlockSubsidy(0, nHeight, c, false);
        BOOST_CHECK(nSubsidy <= 5 * COIN);
        nSum += nSubsidy * 1000;
        BOOST_CHECK(MoneyRange(nSum));
    }
    BOOST_CHECK(nSum > 0);
    BOOST_CHECK(nSum <= MAX_MONEY);
}

BOOST_AUTO_TEST_SUITE_END()
