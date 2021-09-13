#!/bin/sh

set -e

# Args
IPADDR="$1"
SEQID="$2"
FILE="$3"

# Guard: IP must be set
if [[ -z "$3" ]]; then
    echo "Usage:"
    echo "load.sh <ip> <sequence_int> <filename.json>"
    exit 1
fi

# Read file
PAYLOAD="$(cat $FILE)"

# Load, then play
curl "$PAYLOAD" -H "Content-Type: application/json" http://$IPADDR/api/sequence?id=$SEQID
echo ""
echo ""
curl http://$IPADDR/api/sequence?id=$SEQID
echo ""
echo ""
curl --request POST http://$IPADDR/api/play?id=$SEQID
echo "Done."


