# Neoxa Core <img src="https://github.com/0x00ASTRA/imgs/blob/da8ce370006ff3cf71a45b8f3bc0c744dccb573a/crypto/NEOX.png" width=30>

---

Visit our website:

[Neoxa.net](https://neoxa.net)

## Navigation

> [Introduction](#introduction)
>
> [Network at a glance](#network-at-a-glance)
>
> [Installation](#installation)
>
> [Running a Smartnode](#running-a-smartnode)
>
> [License](#license)
>
> [Development](#development)

---

## Introduction

Welcome to the Neoxa Project! Below you can find general information about the project including installation instructions and development insight. Thank you for supporting Neoxa!

### What is Neoxa?

Neoxa is a community-secured Layer 1 blockchain that combines KawPoW GPU mining, a Smartnode service tier, native digital asset issuance, and a Play to Earn gaming layer. New blocks arrive roughly every 60 seconds, and each block reward is split 45% to miners, 45% to Smartnodes, 5% to gaming, and 5% to development, so every party that helps run the network shares in its growth. On top of the base chain, anyone can mint their own native asset without writing a smart contract, and gamers can earn NEOX in real time across supported game servers via Neoxa Gaming. For the full technical picture, please refer to the [Neoxa Whitepaper](https://neoxa.net/whitepaper).

### About Neoxa

Neoxa is dedicated to giving gamers, miners, and operators a real working blockchain to build on. Our team is made up of experienced developers with a deep passion for gaming, and we use the flexibility of modern PC gaming, modding hooks, and dedicated game servers to wire blockchain rewards directly into gameplay people actually play.

In contrast to projects that announce features they never ship, Neoxa stands apart by delivering working products from day one and continuing to ship: a live KawPoW mining network, thousands of active Smartnodes, a native asset protocol, the [Neoxa Exchange](https://neoxa.exchange), wNEOX bridges to Ethereum and Solana, a community block explorer, and a [Play to Earn gaming platform](https://neoxagaming.com) that pays out in NEOX. We believe in demonstrating feasibility through code that runs in production, not slideware.

### NEOX | Neoxa Coin

Neoxa (NEOX) is the native currency of the chain and serves investors, gamers, and developers alike. It pays miners and Smartnodes for securing the network, settles on-chain fees, funds native asset issuance, and is distributed as live in-game rewards through Play to Earn integrations on [Neoxa Gaming](https://neoxagaming.com). NEOX can be traded on [Neoxa Exchange](https://neoxa.exchange) and bridged to Ethereum and Solana as wNEOX via [neoxa.exchange/swap](https://neoxa.exchange/swap), letting the same supply plug into the broader DeFi ecosystem. The base block reward starts at 5,000 NEOX and halves every 2,100,000 blocks, asymptotically capping the total supply at 21 billion NEOX. For more information visit the [Neoxa Website](https://neoxa.net).

---

## Network at a glance

| Parameter | Value |
| --- | --- |
| Consensus | KawPoW (Proof of Work) |
| Target block time | 60 seconds |
| Base block reward | 5,000 NEOX |
| Reward split | 45% miner, 45% Smartnode, 5% gaming, 5% development |
| Halving interval | Every 2,100,000 blocks |
| Maximum supply | 21,000,000,000 NEOX (asymptotic) |
| Smartnode collateral | 1,000,000 NEOX |
| Native asset protocol | Yes (issue, transfer, burn, restricted) |
| InstantSend | Active |
| ChainLocks | Active |
| P2P port | 8788 |
| Default RPC port | 15419 |
| P2PKH address prefix | N |

For the full protocol description see the [Neoxa Whitepaper](https://neoxa.net/whitepaper) and the [Learn](https://neoxa.net/learn) section of the website.

---

## Installation

### Neoxa Core Wallet

Below you will find instructions for installing the Neoxa Core daemon and the Neoxa-QT GUI wallet.

#### Install the latest precompiled executables

Releases are published at [Neoxa Releases](https://github.com/NeoxaChain/Neoxa/releases).

##### Windows 10/11

- Download `neoxad.x.x.x.x-win64.zip` for the daemon, or `neoxaqt.x.x.x.x-win64.zip` for the GUI wallet.
- Extract the contents.
- For the GUI wallet, drag the `neoxa-qt` file into the `neoxad.x.x.x.x-win64/` folder.
- Run the `neoxa-qt` application.

##### macOS

- Download `neoxad.x.x.x.x-osx.zip` for the daemon, or `neoxaqt.x.x.x.x-osx.zip` for the GUI wallet.
- Extract the contents.
- For the GUI wallet, drag the `neoxa-qt` file into the `neoxad.x.x.x.x-osx/` folder.
- Open a terminal in that folder.
- Make the files executable:
  ```
  chmod +x *
  ```
- Run the application:
  ```
  ./neoxa-qt
  ```

##### GNU/Linux

- Download `neoxad.x.x.x.x-linux64.zip` for the daemon, or `neoxaqt.x.x.x.x-linux64.zip` for the GUI wallet.

- Open a terminal in your `Downloads/` folder.

- Create a folder for the binaries:
  ```
  mkdir Neoxa-Wallet
  ```

- Install `unzip` if you do not already have it.

  Ubuntu / Debian:
  ```
  sudo apt-get -y update && sudo apt install -y unzip
  ```
  Fedora:
  ```
  sudo dnf -y install unzip
  ```
  Arch / Manjaro:
  ```
  sudo pacman -S unzip
  ```

- Extract the daemon:
  ```
  unzip neoxad.x.x.x.x-linux64.zip -d ./Neoxa-Wallet
  ```

- For the GUI wallet, also extract:
  ```
  unzip neoxaqt.x.x.x.x-linux64.zip -d ./Neoxa-Wallet
  ```

- Move into the folder and make the files executable:
  ```
  cd Neoxa-Wallet
  chmod +x *
  ```

- Run the GUI wallet:
  ```
  ./neoxa-qt
  ```

### Minimal neoxa.conf

If you are running the daemon for the first time, create `~/.neoxacore/neoxa.conf` (Linux/macOS) or `%APPDATA%\NeoxaCore\neoxa.conf` (Windows) with at minimum:

```
server=1
listen=1
rpcuser=changeMe
rpcpassword=longRandomStringYouChooseHere
rpcallowip=127.0.0.1
```

Enable additional indexes only if you need them. They increase disk usage and the time required for the initial sync:

```
txindex=1         # required for getrawtransaction and explorer-style lookups
addressindex=1    # required for getaddress* calls (balance / history by address)
assetindex=1      # required for getassetdata and full asset RPC support
spentindex=1      # required for tracing spent outputs (used by block explorers)
```

---

## Running a Smartnode

A Smartnode is a node that locks 1,000,000 NEOX as collateral and provides services to the network in exchange for 45% of every block reward. Smartnodes power InstantSend, ChainLocks, and chain governance.

Full step-by-step setup instructions are maintained on the developer site:

**[dev.neoxa.net/smartnode](https://dev.neoxa.net/smartnode)**

A live count of active Smartnodes plus a reward calculator are available on the [Neoxa website](https://neoxa.net/smartnodes).

---

## License

Neoxa Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

---

## Development

### Development process

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/NeoxaChain/Neoxa/tags) are created
regularly to indicate new official, stable release versions of Neoxa Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

### Building from source

Build instructions for each supported platform live in the [`doc/`](doc/) directory:

- [`doc/build-unix.md`](doc/build-unix.md)
- [`doc/build-osx.md`](doc/build-osx.md)
- [`doc/build-windows.md`](doc/build-windows.md)

### Lineage

Neoxa Core is a Dash-derived chain (DIP-3 ProTx, LLMQ, InstantSend, ChainLocks, governance), with KawPoW consensus and the native asset protocol both ported from Ravencoin Core. Familiarity with the Dash and Ravencoin codebases is helpful when working on consensus-level changes.

### Testing

Neoxa Core is committed to making sure every release is thoroughly tested before reaching mainnet. A public testnet runs alongside the main chain, and the unit and functional test suites bundled with the codebase are run on each pull request and tag.

#### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

---

### Appreciation

Thank you to the Bitcoin developers.
Thank you to the Dash developers.
Thank you to the Ravencoin developers.
