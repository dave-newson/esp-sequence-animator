#!/bin/sh

set -e

# Args
IPADDR="$1"
SEQID="$2"
FILE="$3"

# Guard: IP must be set
if [[ -z "$2" ]]; then
    echo "Animation Sequence Loader"
    echo "Play only: ./load.sh <ip> <sequence_int>"
    echo "Write JSON to device: ./load.sh <ip> <sequence_int> <filename.json>"
    exit 1
fi

# Write to device if file specified
if [[ $FILE ]]; then
    echo "Loading $FILE to device ..."

    # Read file
    PAYLOAD="$(cat $FILE)"

    # Write
    curl -s --request POST -H "Content-Type: application/json" http://$IPADDR/api/sequence?id=$SEQID --data "$PAYLOAD"
    echo ""
    curl -s http://$IPADDR/api/sequence?id=$SEQID
fi

echo "Starting sequence $SEQID ..."
curl -s --request POST http://$IPADDR/api/play?id=$SEQID
echo ""
echo "Done."


