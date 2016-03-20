set -eux
pushd 103033110_01/simulator
    make clean
popd
tar czf 103033110_01.tar.gz 103033110_01
