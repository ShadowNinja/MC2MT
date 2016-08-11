#include "MCMap.hpp"
#include "MTMap.hpp"
#include "threads.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <unistd.h>
#endif

using timer = std::chrono::steady_clock;


int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl;
		return 1;
	}

	timer::time_point start = timer::now();

	MCMap mc_map(argv[1]);
	MTMap mt_map(argv[2]);

	std::vector<MCGroup*> groups;
	mc_map.listGroups(groups);

	for (MCGroup* g : groups)
		convert_queue.q.push(g);

	init_threads(&mc_map, &mt_map);

	bool is_tty = isatty(fileno(stderr));

	while (groups_done < groups.size()) {
		size_t dt = std::chrono::duration_cast<std::chrono::seconds>(
				timer::now() - start).count();
		if (dt == 0)
			dt = 1;

		std::cerr
			<< " Converted "
			<< groups_done << '/' << groups.size()
			<< " groups, " << blocks_done
			<< " blocks.  " << blocks_done / dt
			<< " blocks/sec";
		if (is_tty)
			std::cerr << "        \r" << std::flush;
		else
			std::cerr << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	deinit_threads();

	int32_t total_s = std::chrono::duration_cast<std::chrono::seconds>(
			timer::now() - start).count();
	std::cerr << (is_tty ? '\r' : '\n')
		<< "Converted " << blocks_done << " blocks in " << groups_done
		<< " groups in " <<  total_s << "s.                " << std::endl;

	std::set<std::string> *unknown;
	MTMap::getUnknown(&unknown);
	if (!unknown->empty()) {
		std::cerr << "Unknown MC items:";
		for (const std::string & name : *unknown)
			std::cerr << ' ' << name;
		std::cerr << std::endl;
	}


	return 0;
}
