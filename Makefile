.DEFAULT_GOAL := help

.PHONY: conf run clean

TOP := ../..
ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

WAF_CONF_FLAGS := -d debug --enable-examples --enable-tests

conf: ; ( cd $(TOP) && ./waf $(WAF_CONF_FLAGS) configure )

run: ; ../../waf --run llt

run-debug: ; ../../waf --run llt --command-template="gdb --args %s"

clean: ; ( cd $(TOP) && $(RM) *txt *pcap *tr *sca )

help:
	@echo
	@echo Available targets:
	@echo
	@echo conf:     configure
	@echo run:      run the LLT experiment
	@echo clean:    delete experiment traces
	@echo
