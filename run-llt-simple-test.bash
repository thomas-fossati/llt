#!/bin/bash

set -eu
set -o pipefail

# interesting log tags:
# - LltRealtimeApp
# - PfFfMacScheduler
# - EpcSgwPgwApplication
# - EpcTftClassifier
# - EpcTft

function main() {
  for marking in "false" "true"
  do
    local tag="llt-simple-marking-${marking}"
    echo ">> Running trial with marking ${marking}"
    NS_LOG="LLTSimple" ../../waf --run "llt --marking-enabled=${marking} --run=${tag}"
  done
}

main $*

# vim: ai ts=2 sw=2 et sts=2 ft=sh
