#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

FILOPOWD=${FILOPOWD:-$SRCDIR/filopowd}
FILOPOWCLI=${FILOPOWCLI:-$SRCDIR/filopow-cli}
FILOPOWTX=${FILOPOWTX:-$SRCDIR/filopow-tx}
FILOPOWQT=${FILOPOWQT:-$SRCDIR/qt/filopow-qt}

[ ! -x $FILOPOWD ] && echo "$FILOPOWD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
FPOWVER=($($FILOPOWCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for filopowd if --version-string is not set,
# but has different outcomes for filopow-qt and filopow-cli.
echo "[COPYRIGHT]" > footer.h2m
$FILOPOWD --version | sed -n '1!p' >> footer.h2m

for cmd in $FILOPOWD $FILOPOWCLI $FILOPOWTX $FILOPOWQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${FPOWVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${FPOWVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
