# DIL Unit Testing Guide

This guide explains how to run the DIL (DikuMUD Interactive Language) unit tests for the VME MUD server.

## Overview

The DIL unit tests validate the functionality of DIL scripts and ensure the MUD server's scripting system works correctly. The tests are located in the `test.zon` file and are automatically executed when the server starts with the test configuration.

## Prerequisites

- Access to the VME MUD server source code
- Compiled VME binaries (especially `vmc` and `vme`)
- Proper file permissions in the vme directory structure

## Running the Tests

Follow these steps to execute the DIL unit tests:

### 1. Compile Zone Files
```bash
cd vme/zone/
../vme/bin/vmc -I../include/ -m *.zon
```
This compiles all zone files including the test zone containing the unit tests.

### 2. Configure Test Environment
```bash
cd vme/etc/
cp zonelist.test zonelist
```
This replaces the production zonelist with the test configuration that only loads essential zones for testing.

### 3. Run the Test Suite
```bash
cd vme/bin/
./vme
```
Start the VME server. It will automatically run all DIL unit tests and exit.

### 4. Monitor Progress (Optional)
In a separate terminal, you can monitor the log:
```bash
cd vme/bin/
tail -f vme.log
```

## Understanding the Results

### Success Indicators
- **Exit code 0**: All tests passed successfully
- **Log messages**: Look for "SUCCESS" or "Done" messages for each test (T001-T016)
- **Graceful shutdown**: Server should exit with "Received signal #15" after completing tests

### Test Coverage
The unit tests cover various DIL functionality:
- **T001**: Follower behavior and movement
- **T002**: Data manipulation and validation
- **T003**: Core functionality tests
- **T004**: Basic operations
- **T005**: Advanced functionality
- **T006-T009**: Various DIL features
- **T010-T016**: Edge cases and specific functionality

### Common Log Messages
- `test/T###//INFO`: Test progress information
- `test/T###//SUCCESS`: Test completed successfully
- `test/T###//Done`: Test finished
- `Lab director launching test`: Test runner starting next test

## Troubleshooting

### Common Issues

1. **vmc not found**
   - Ensure the VME binaries are compiled
   - Check file permissions: `chmod +x vme/bin/vmc`

2. **Permission denied**
   - Ensure proper ownership: `chown -R $USER:$USER vme/`
   - Check execute permissions on binaries

3. **Missing files**
   - Verify `zonelist.test` exists in `vme/etc/`
   - Ensure all zone files are present in `vme/zone/`

4. **Test failures**
   - Check `vme.log` for detailed error messages
   - Look for DIL compilation errors or runtime exceptions

### Expected Warnings
Some warnings are normal during testing:
- Missing board index files (wiz.idx, builder.idx, etc.)
- Missing DIL templates for unused zones
- External reference resolution warnings (these don't affect tests)

## Restoring Production

After testing, restore the production configuration:
```bash
cd vme/etc/
cp zonelist.minimal zonelist  # or your production zonelist
```

## Test Files

- **Test zone**: `vme/zone/test.zon` - Contains all DIL unit test templates
- **Test config**: `vme/etc/zonelist.test` - Minimal zone list for testing
- **Log file**: `vme/bin/vme.log` - Detailed test execution log

## Automation

For automated testing, you can create a script:
```bash
#!/bin/bash
cd vme/zone/
../vme/bin/vmc -I../include/ -m *.zon && \
cd ../etc/ && \
cp zonelist.test zonelist && \
cd ../bin/ && \
./vme
if [ $? -eq 0 ]; then
    echo "All DIL tests passed!"
else
    echo "DIL tests failed. Check vme.log for details."
    exit 1
fi
```

This ensures consistent test execution and proper error handling.