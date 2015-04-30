
#include <tina++/utils/base64.h>
#include <tina++/debug/graph.h>

#include <atomic>


namespace TURAG {
namespace Debug {

Graph::graphCount(0);
Graph::allEnabled(TURAG_DEBUG_ENABLE_GRAPH_DEFAULT ? true : false);


Graph::Graph(const char *name, bool enabled_) :
	channels(0), enabled(enabled_)
{
	index = graphCount.fetch_add(1);
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CREATE TURAG_DEBUG_GRAPH_PREFIX "%u %s" TURAG_DEBUG_NEWLINE, index, name);
}

void Graph::doPlot(float time, float* data, size_t count) {
	if (count > channels) {
		turag_errorf("GraphBase::doPlot: got %d elements, but only %d channels are defined", count, channels);
	} else if (enabled && allEnabled) {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

		uint8_t encoded[7] = {0};

		turag_base64_encode((uint8_t*)&time, 4, encoded);
		turag_debug_puts((char*)encoded);

		int i;
		for ( i = 0; i < count; ++i ) {
			turag_base64_encode((uint8_t*)data, 4, encoded);
			turag_debug_puts((char*)encoded);
			++data;
		}

		turag_debug_puts(TURAG_DEBUG_NEWLINE);
	}
}

void Graph::doPlot2D(float* data, size_t count) {
	if (count > channels) {
		turag_errorf("GraphBase::plot: got elements for %d channels, but only %d channels are defined", count, channels);
	} else if (enabled && allEnabled) {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA2D TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

		uint8_t encoded[7] = {0};

		int i;
		for ( i = 0; i < count * 2; ++i ) {
			turag_base64_encode((uint8_t*)data, 4, encoded);
			turag_debug_puts((char*)encoded);
			++data;
		}

		turag_debug_puts(TURAG_DEBUG_NEWLINE);
	}
}

void Graph::addChannelGroup(const char* label, std::initializer_list<unsigned> channelIndices) {
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNELGROUP TURAG_DEBUG_GRAPH_PREFIX "%u %u", index, channelIndices.size());

	for (unsigned i : channelIndices) {
		turag_debug_printf(" %d", i);
	}

	turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, label);
}





GraphAuto::GraphAuto(const char *name, std::initializer_list<const char*> channelNames, bool enabled) :
	Graph(name, enabled)
{
	for (const char* channel_title : channelNames) {
		addChannel(channel_title);
	}
}

GraphAuto::GraphAuto(const char *name, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled) :
	Graph(name, enabled)
{
	static_assert(channelNames.size() == channelGroups.size(), "channelNames.size() == channelGroups.size() not true");

	auto groupIterator = channelGroups.begin();

	for (auto&& list : channelNames) {
		unsigned channel_begin = channels;

		for (const char* channel_title : list) {
			addChannel(channel_title);
		}

		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNELGROUP TURAG_DEBUG_GRAPH_PREFIX "%u %u", index, channels - channel_begin);
		for (unsigned i = channel_begin; i < channels; ++i) {
			turag_debug_printf(" %d", i);
		}
		turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, *groupIterator);
		++groupIterator;
	}
}

void GraphAuto::addChannel(const char* title) {
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d 0 %s" TURAG_DEBUG_NEWLINE,
					   index, title);
	++channels;
}





GraphFixedSize::GraphFixedSize(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<const char*> channelNames, bool enabled = true) :
	Graph(name, enabled)
{
	for (const char* channel_title : channelNames) {
		addChannel(channel_title, x_left, y_bottom, width, height);
	}
}

GraphFixedSize::GraphFixedSize(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled) :
	Graph(name, enabled)
{
	static_assert(channelNames.size() == channelGroups.size(), "channelNames.size() == channelGroups.size() not true");

	auto groupIterator = channelGroups.begin();

	for (auto&& list : channelNames) {
		unsigned channel_begin = channels;

		for (const char* channel_title : list) {
			addChannel(channel_title, x_left, y_bottom, width, height);
		}

		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNELGROUP TURAG_DEBUG_GRAPH_PREFIX "%u %u", index, channels - channel_begin);
		for (unsigned i = channel_begin; i < channels; ++i) {
			turag_debug_printf(" %d", i);
		}
		turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, *groupIterator);
		++groupIterator;
	}
}

void GraphFixedSize::addChannel(const char* title, int x_left, int y_bottom, unsigned width, unsigned height) {
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL_FIXED TURAG_DEBUG_GRAPH_PREFIX "%d %d %d %d %d %s" TURAG_DEBUG_NEWLINE,
					   index, x_left, y_bottom, width, height, title);
	++channels;
}




GraphFixedTime::GraphFixedTime(const char *name, unsigned time, std::initializer_list<const char*> channelNames, bool enabled) :
	Graph(name, enabled)
{
	for (const char* channel_title : channelNames) {
		addChannel(channel_title, time);
	}
}

GraphFixedTime::GraphFixedTime(const char *name, unsigned time, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled) :
	Graph(name, enabled)
{
	static_assert(channelNames.size() == channelGroups.size(), "channelNames.size() == channelGroups.size() not true");

	auto groupIterator = channelGroups.begin();

	for (auto&& list : channelNames) {
		unsigned channel_begin = channels;

		for (const char* channel_title : list) {
			addChannel(channel_title, time);
		}

		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNELGROUP TURAG_DEBUG_GRAPH_PREFIX "%u %u", index, channels - channel_begin);
		for (unsigned i = channel_begin; i < channels; ++i) {
			turag_debug_printf(" %d", i);
		}
		turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, *groupIterator);
		++groupIterator;
	}
}

void GraphFixedTime::addChannel(const char* title, unsigned time) {
	turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d %d %s" TURAG_DEBUG_NEWLINE,
					   index, time, title);
	++channels;
}






//extern "C" void turag_graph_add_vertical_marker(unsigned index, const char* label, float time) {
//	if (!index) {
//		return;
//	}
	
//    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_VERTICAL_MARKER TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

//    uint8_t encoded[7] = {0};

//    turag_base64_encode((uint8_t*)&time, 4, encoded);
//    turag_debug_puts((char*)encoded);

//	turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, label);
//}






} // namespace Debug
} // namespace TURAG


