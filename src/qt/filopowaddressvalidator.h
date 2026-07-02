// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Raven Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef FILOPOW_QT_FILOPOWADDRESSVALIDATOR_H
#define FILOPOW_QT_FILOPOWADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class FilopowAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit FilopowAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Filopow address widget validator, checks for a valid filopow address.
 */
class FilopowAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit FilopowAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // FILOPOW_QT_FILOPOWADDRESSVALIDATOR_H
