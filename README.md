FILOPOW Core integration/staging tree
=====================================

https://github.com/filoproject/filopow

To see how to run FILOPOW, please read the respective files in [the doc folder](doc).

Key documents:

- [doc/filopow-params.md](doc/filopow-params.md) — network parameters, emission
  schedule, ports, address format.
- [doc/mining.md](doc/mining.md) — how to point a GPU miner at FILOPOW.
- [doc/fair-launch.md](doc/fair-launch.md) — the no-premine commitments and how
  to verify them; security policy.
- [doc/launch-runbook.md](doc/launch-runbook.md) — operator checklist to launch
  the chain.


What is FILOPOW?
----------------

FILOPOW is a standalone, GPU-mineable proof-of-work cryptocurrency. It is a fork
of [Ravencoin](https://github.com/RavenProject/Ravencoin) and uses the **KawPow**
mining algorithm, so the existing GPU-mining ecosystem (miners, pools, and tools
built for KawPow coins) works with it directly.

FILOPOW keeps Ravencoin's on-chain **asset layer**, so anyone can issue and
transfer tokens on the network, but its parameters are its own:

- **Ticker:** FPOW
- **Algorithm:** KawPow (GPU), active from the first mined block
- **Block time:** 1 minute
- **Block reward:** 5 FPOW, halving every 2,100,000 blocks (~4 years)
- **Maximum supply:** ~21,000,000 FPOW
- **Addresses:** start with `F`

FILOPOW uses peer-to-peer technology to operate with no central authority:
managing transactions and issuing money are carried out collectively by the
network.


Fair launch
-----------

FILOPOW launches with **no premine, no developer fee, no founder allocation, and
no other set-aside**. Every coin is mined by the public. The mining start time
is announced publicly in advance, with source code and binaries published before
genesis, so no one — including the project's own operators — can mine ahead of
the announced start. The reason FILOPOW exists is to bring a mining community
into the wider FILO ecosystem; the thing being earned with a fair launch is that
community's trust, and it is protected in every decision.


License
-------

FILOPOW Core is released under the terms of the MIT license. See [COPYING](COPYING)
for more information or see https://opensource.org/licenses/MIT.

FILOPOW is a fork of Raven Core; the original Raven Core and Bitcoin Core
developers retain their copyright over the code that predates this fork.


Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/filoproject/filopow/tags) are
created regularly to indicate new official, stable release versions of FILOPOW
Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).


Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help
out by testing other people's pull requests, and remember this is a
security-critical project where any mistake might cost people lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for
new code, and to submit new unit tests for old code. Unit tests can be compiled
and run (assuming they weren't disabled in configure) with: `make check`.
Further details on running and extending unit tests can be found in
[/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written in Python,
that are run automatically on the build server. These tests can be run (if the
[test dependencies](/test) are installed) with: `test/functional/test_runner.py`.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is not
straightforward.


Acknowledgements
----------------

FILOPOW is built on the work of the Ravencoin and Bitcoin developers. It exists
because of the code they wrote and open-sourced, and we are grateful for it.
