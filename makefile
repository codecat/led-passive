DEPLOY_IP=192.168.0.133
DEPLOY_USER=ubuntu
DEPLOY_TARGET=/home/ubuntu/led-passive

cross-build: build-arm
	cd build-arm && make -j9

build-arm:
	mkdir build-arm
	cd build-arm && cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-arm.cmake

deploy: cross-build
	-ssh $(DEPLOY_USER)@$(DEPLOY_IP) "killall led-passive"
	scp build-arm/led-passive $(DEPLOY_USER)@$(DEPLOY_IP):$(DEPLOY_TARGET)/
#scp -r data/* $(DEPLOY_USER)@$(DEPLOY_IP):$(DEPLOY_TARGET)/

test: deploy
	ssh -t $(DEPLOY_USER)@$(DEPLOY_IP) "cd $(DEPLOY_TARGET); ./led-passive"

debug: deploy
	ssh -t $(DEPLOY_USER)@$(DEPLOY_IP) "cd $(DEPLOY_TARGET); gdb ./led-passive"

clean:
	rm -rf build-arm
