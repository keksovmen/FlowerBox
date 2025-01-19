#include <iostream>
#include <cassert>
#include <random>

#include "../main/include/fb_buffer.hpp"




static void _test_initial()
{
	fb::util::Buffer<4096> buffer;

	assert(!buffer.isPartitioned());
	assert(buffer.readPartition().empty());
	assert(buffer.readPartition().data() == buffer.readPartition().data());
}

static void _test_small_write()
{
	fb::util::Buffer<4096> buffer;
	char tmp[512] = {};

	buffer.putData({tmp, sizeof(tmp)});
	assert(!buffer.isPartitioned());
	assert(buffer.readPartition().length() == sizeof(tmp));
	assert(buffer.readPartition().empty());
}

static void _test_sequential_write()
{
	fb::util::Buffer<4096> buffer;
	char tmp[512] = {"loh"};

	const int times = 3;
	for(int i = 0; i < times; i++){
		buffer.putData({tmp, sizeof(tmp)});
	}
	assert(!buffer.isPartitioned());

	auto read = buffer.readPartition();
	assert(read.length() == sizeof(tmp) * times);

	assert(strstr(read.data(), tmp));
	assert(buffer.readPartition().empty());
}

static void _test_overwrite()
{
	fb::util::Buffer<8> buffer;
	char tmp[12] = {"01234567890"};

	buffer.putData({tmp, sizeof(tmp)});
	assert(buffer.isPartitioned());

	auto read = buffer.readPartition();
	assert(read.length() == 4);
	assert(strstr(read.data(), "4567") == read.data());

	assert(!buffer.isPartitioned());
	read = buffer.readPartition();
	assert(read.length() == 4);
	assert(strstr(read.data(), "890") == read.data());
}

static void _test_overwrite_after_read()
{
	fb::util::Buffer<8> buffer;
	char tmp[13] = {"012345678901"};

	buffer.putData({tmp, 12});
	assert(buffer.isPartitioned());

	auto read = buffer.readPartition();
	assert(read.length() == 4);
	assert(read.find("4567") != std::string_view::npos);
	assert(!buffer.isPartitioned());

	buffer.putData({"888888", 6});

	assert(buffer.isPartitioned());

	read = buffer.readPartition();
	assert(read.length() == 6);
	assert(read.find("018888") != std::string_view::npos);


	read = buffer.readPartition();
	assert(read.length() == 2);
	assert(read.find("88") != std::string_view::npos);
}

static void _test_write_more()
{
	fb::util::Buffer<8> buffer;
	char tmp[13] = {"1234"};

	for(int i = 0; i < 8; i++){
		buffer.putData({tmp, 4});
	}

	assert(!buffer.isPartitioned());
	auto read = buffer.readPartition();
	assert(read.length() == 8);

	for(int i = 0; i < 3; i++){
		buffer.putData({tmp, 4});
	}

	assert(buffer.isPartitioned());
	read = buffer.readPartition();
	assert(read.length() == 4);

	for(int i = 0; i < 10; i++){
		buffer.putData({"F", 1});
	}

	assert(buffer.isPartitioned());
	read = buffer.readPartition();
	assert(read.length() == 2);

	assert(!buffer.isPartitioned());
	read = buffer.readPartition();
	assert(read.length() == 6);
}

static void _test_writes_reads()
{
	fb::util::Buffer<512> buffer;
	char tmp[512] = {};

	int put_count = 0;
	int read_count = 0;
	for(int i = 0; i < 60; i++){
		int size = sizeof(tmp) - (std::rand() % (sizeof(tmp) / 2));

		buffer.putData({tmp, (size_t) size});
		put_count += size;

		auto part = buffer.readPartition();
		read_count += part.size();
		// std::cout << "Put: " << size << " total: " << put_count << "\tRead: " << part.length();

		part = buffer.readPartition();
		read_count += part.size();
		// std::cout << " + " << part.length() << " total: " << read_count << std::endl;
	}

	// std::cout << "Put: " << put_count << "\tRead: " << read_count << std::endl;
	assert(put_count == read_count);
}

static void _test_writes_and_couple_reads()
{
	fb::util::Buffer<4096> buffer;
	char tmp[512] = {};

	int put_count = 0;
	int read_count = 0;
	for(int i = 0; i < 60; i++){
		int size = sizeof(tmp) - (std::rand() % (sizeof(tmp) / 2));

		buffer.putData({tmp, (size_t) size});
		put_count += size;

		// auto part = buffer.readPartition();
		// read_count += part.size();
		// std::cout << "Put: " << size << " total: " << put_count << "\tRead: " << part.length();

		// part = buffer.readPartition();
		// read_count += part.size();
		// std::cout << " + " << part.length() << " total: " << read_count << std::endl;
	}

	auto part = buffer.readPartition();
	read_count += part.size();

	part = buffer.readPartition();
	read_count += part.size();

	assert(read_count == 4096);

	// std::cout << "Put: " << put_count << "\tRead: " << read_count << std::endl;
	// assert(put_count == read_count);
}


int main()
{
	_test_initial();
	_test_small_write();
	_test_sequential_write();
	_test_overwrite();
	_test_overwrite_after_read();
	_test_write_more();
	_test_writes_reads();
	_test_writes_and_couple_reads();
}