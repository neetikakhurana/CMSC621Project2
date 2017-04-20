#!/bin/bash
for i in {0..3}
{
	./nodes 127.0.0.1 8888 $((i))&
}
exit 0