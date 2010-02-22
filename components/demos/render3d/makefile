include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

build: build.demo
run: run.demo
#clean: clean.demo

build.demo:
	@$(MAKE) build -C $(COMPONENT_DIR)/game
	
run.demo: build.demo
	@$(MAKE) run -C $(COMPONENT_DIR)/game

clean.demo: clean.demo
	@$(MAKE) clean -C $(COMPONENT_DIR)/game

.PHONY: build.demo run.demo clean.demo
