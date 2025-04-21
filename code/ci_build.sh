name: CI/CD Pipeline

on:
  push:
    branches:
      - main
  pull_request:
    branches:
      - main

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Install dependencies
        working-directory: code/
        run: |
          chmod +x ./init.sh
          ./init.sh

      - name: Symlink libsndfile to /usr/lib
        run: sudo ln -s /lib/x86_64-linux-gnu/libsndfile.so.1 /usr/lib/libsndfile.so.1

      - name: Build
        working-directory: code/
        run: |
          chmod +x ./ci_build.sh
          ./build.sh > build.log 2>&1 || (cat build.log && exit 1)

      - name: Upload build log
        uses: actions/upload-artifact@v4
        with:
          name: build-log
          path: build.log

      - name: Upload build artifacts
        uses: actions/upload-artifact@v4
        with:
          name: build
          path: code/build/

  test:
    runs-on: ubuntu-latest
    needs: build
    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Download build artifacts
        uses: actions/download-artifact@v4
        with:
          name: build
          path: code/build

      - name: Run unit tests
        working-directory: code/
        run: |
          chmod +x ./build/src/test/unit_tests/unit_tests
          chmod +x ./unit_test.sh
          sudo ./unit_test.sh 

  lint:
    runs-on: ubuntu-latest
    needs: build
    continue-on-error: true
    steps:
      - name: Download build log
        uses: actions/download-artifact@v4
        with:
          name: build-log
          path: .

      - name: Check for warnings in the build log
        run: |
          cat build.log
          ! grep -q "warning:" build.log
