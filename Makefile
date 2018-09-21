.DEFAULT_GOAL := help

.PHONY: conf run clean

TOP := ../..
ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

WAF_CONF_FLAGS := -d debug --enable-examples --enable-tests

SIM_CONFIG := $(ROOT_DIR)/default.conf
SIM_TEMPLATE := $(ROOT_DIR)/default.tmpl

conf: ; ( cd $(TOP) && ./waf $(WAF_CONF_FLAGS) configure )

run: ; ../../waf --run llt --command-template="%s --ns3::ConfigStore::Filename=$(SIM_CONF) --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText"

run-debug: ; ../../waf --run llt --command-template="gdb --args %s"

template: ; ../../waf --run llt --command-template="%s --ns3::ConfigStore::Filename=$(SIM_TEMPLATE) --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText"

clean: ; ( cd $(TOP) && $(RM) *txt *pcap *tr )

help:
	@echo
	@echo Available targets:
	@echo
	@echo conf:     configure
	@echo run:      run the LLT experiment
	@echo clean:    delete experiment traces
	@echo template: dump configuration template to default.tmpl
	@echo
