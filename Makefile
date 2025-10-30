# Makefile for OneLake ODBC test applications
# Supports RHEL8 ARM64 and x86_64 architectures

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lodbc

# Detect architecture
ARCH := $(shell uname -m)

# Target executables
TARGETS = ConsecutiveConnection_Issue ConsecutiveConnection_Successful

.PHONY: all clean

all: $(TARGETS)

ConsecutiveConnection_Issue: ConsecutiveConnection_Issue.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

ConsecutiveConnection_Successful: ConsecutiveConnection_Successful.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)

info:
	@echo "Building for architecture: $(ARCH)"
	@echo "Compiler: $(CXX)"
	@echo "Targets: $(TARGETS)"
