#!/bin/sh

$UPCXX_INSTALL/bin/upcxx-run -n1 ../build/pgas-graph-test --vertex-count=64 &>> log.txt && 
$UPCXX_INSTALL/bin/upcxx-run -n2 ../build/pgas-graph-test --vertex-count=64 &>> log.txt &&
$UPCXX_INSTALL/bin/upcxx-run -n4 ../build/pgas-graph-test --vertex-count=64 &>> log.txt &&
echo "**********" >> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n1 ../build/pgas-graph-test --vertex-count=128 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n2 ../build/pgas-graph-test --vertex-count=128 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n4 ../build/pgas-graph-test --vertex-count=128 &>> log.txt ;
# echo "**********" >> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n1 ../build/pgas-graph-test --vertex-count=256 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n2 ../build/pgas-graph-test --vertex-count=256 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n4 ../build/pgas-graph-test --vertex-count=256 &>> log.txt ;
# echo "**********" >> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n1 ../build/pgas-graph-test --vertex-count=512 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n2 ../build/pgas-graph-test --vertex-count=512 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n4 ../build/pgas-graph-test --vertex-count=512 &>> log.txt ;
# echo "**********" >> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n1 ../build/pgas-graph-test --vertex-count=1024 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n2 ../build/pgas-graph-test --vertex-count=1024 &>> log.txt ;
# $UPCXX_INSTALL/bin/upcxx-run -n4 ../build/pgas-graph-test --vertex-count=1024 &>> log.txt ;
