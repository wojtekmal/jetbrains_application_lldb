./build/bin/gwatch --var global_int_to_watch --exec ./build/bin/test_int_incrementation > invocation_test_output.txt
diff invocation_test_output.txt invocation_test_proper_output.txt
if [ $? -eq 0 ]
then
    echo "test passed"
else
    echo "test failed"
fi
rm invocation_test_output.txt