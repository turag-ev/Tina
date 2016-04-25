
#include <tina/debug/print-impl.h>
#include <tina/debug/internal-debug.h>
#include <tina++/utils/base64.h>
#include <tina++/debug/graph.h>

#include <atomic>


namespace TURAG {
namespace Debug {

std::atomic<unsigned> Graph::graphCount{1};
bool Graph::allEnabled{TURAG_DEBUG_ENABLE_GRAPH_DEFAULT ? true : false};


void Graph::init(const char *name_, bool enabled_) {
	if (isInitialized()) {
        turag_internal_warning("ignored call Graph::init on already initialized instance");
	} else {
		channels = 0;
		enabled = enabled_;
		name = name_;
		index = graphCount.fetch_add(1);
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CREATE TURAG_DEBUG_GRAPH_PREFIX "%u %s" TURAG_DEBUG_NEWLINE, index, name);
	}
}

void Graph::startNewDiagram(void) {
	if (!isInitialized()) {
        turag_internal_error("tried to call Graph::startNewDiagram prior to initialization");
	} else if (enabled && allEnabled) {
		// we change index to the new index after sending the copy command to circumvent threading problems
		unsigned new_index = graphCount.fetch_add(1);
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_COPY TURAG_DEBUG_GRAPH_PREFIX "%u %u %s" TURAG_DEBUG_NEWLINE, new_index, index, name);
		index = new_index;
	}
}

void Graph::plot(float time, float* data, size_t count) {
	if (!isInitialized()) {
        turag_internal_error("tried to call Graph::plot prior to initialization");
	} else if (count > channels) {
        turag_internal_errorf("GraphBase::plot: got %d elements, but only %d channels are defined", (int)count, channels);
	} else if (enabled && allEnabled) {
		if (startNewDiagramOnTimeOverflow && time < lastTime) {
			startNewDiagram();
		}
		lastTime = time;

		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA TURAG_DEBUG_GRAPH_PREFIX "%u ", index);

		uint8_t encoded[7] = {0};

		turag_base64_encode((uint8_t*)&time, 4, encoded);
		turag_debug_puts((char*)encoded);

		for ( unsigned i = 0; i < count; ++i ) {
			turag_base64_encode((uint8_t*)data, 4, encoded);
			turag_debug_puts((char*)encoded);
			++data;
		}

		turag_debug_puts(TURAG_DEBUG_NEWLINE);
	}
}

void Graph::plot2D(unsigned channelIndex, float x, float y) {
	if (!isInitialized()) {
        turag_internal_error("tried to call Graph::plot2D prior to initialization");
	} else if (channelIndex > channels) {
        turag_internal_errorf("GraphBase::plot2D: channel with index %u not defined", channelIndex);
	} else if (enabled && allEnabled) {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_DATA2D TURAG_DEBUG_GRAPH_PREFIX "%u %u ", index, channelIndex);

		uint8_t encoded[7] = {0};

		turag_base64_encode((uint8_t*)&x, 4, encoded);
		turag_debug_puts((char*)encoded);
		turag_base64_encode((uint8_t*)&y, 4, encoded);
		turag_debug_puts((char*)encoded);
		
		turag_debug_puts(TURAG_DEBUG_NEWLINE);
	}
}

void Graph::addChannelGroup(const char* label, std::initializer_list<unsigned> channelIndices) {
	if (!isInitialized()) {
        turag_internal_error("tried to call Graph::addChannelGroup prior to initialization");
	} else {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNELGROUP TURAG_DEBUG_GRAPH_PREFIX "%u %u", index, (unsigned)channelIndices.size());

		for (unsigned i : channelIndices) {
			turag_debug_printf(" %d", i);
		}

		turag_debug_printf(" %s" TURAG_DEBUG_NEWLINE, label);
	}
}





void GraphAuto::init(const char *name, std::initializer_list<const char*> channelNames, bool enabled) {
	if (isInitialized()) {
        turag_internal_warning("ignored call GraphAuto::init on already initialized instance");
		return;
	}

	Graph::init(name, enabled);

	for (const char* channel_title : channelNames) {
		addChannel(channel_title);
	}
}

void GraphAuto::init(const char *name, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled) {
	if (isInitialized()) {
        turag_internal_warning("ignored call GraphAuto::init on already initialized instance");
		return;
	}

	Graph::init(name, enabled);

#if __cplusplus >= 201300L
	static_assert(channelNames.size() == channelGroups.size(), "channelNames.size() == channelGroups.size() not true");
#else
	if (channelNames.size() != channelGroups.size()) {
        turag_internal_error("GraphAuto::GraphAuto: channelNames.size() == channelGroups.size() not true");
		return;
	}
#endif

	auto groupIterator = channelGroups.begin();

	for (const auto& list : channelNames) {
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
	if (!isInitialized()) {
        turag_internal_error("tried to call GraphAuto::addChannel prior to initialization");
	} else {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d 0 %s" TURAG_DEBUG_NEWLINE,
						   index, title);
		++channels;
	}
}





void GraphFixedSize::init(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<const char*> channelNames, bool enabled) {
	if (isInitialized()) {
        turag_internal_warning("ignored call GraphFixedSize::init on already initialized instance");
		return;
	}

	Graph::init(name, enabled);

	for (const char* channel_title : channelNames) {
		addChannel(channel_title, x_left, y_bottom, width, height);
	}
}

void GraphFixedSize::init(const char *name, int x_left, int y_bottom, unsigned width, unsigned height, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled) {
	if (isInitialized()) {
        turag_internal_warning("ignored call GraphFixedSize::init on already initialized instance");
		return;
	}

	Graph::init(name, enabled);

#if __cplusplus >= 201300L
	static_assert(channelNames.size() == channelGroups.size(), "channelNames.size() == channelGroups.size() not true");
#else
	if (channelNames.size() != channelGroups.size()) {
        turag_internal_error("GraphAuto::GraphAuto: channelNames.size() == channelGroups.size() not true");
		return;
	}
#endif

	auto groupIterator = channelGroups.begin();

	for (const auto& list : channelNames) {
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
	if (!isInitialized()) {
        turag_internal_error("tried to call GraphFixedSize::addChannel prior to initialization");
	} else {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL_FIXED TURAG_DEBUG_GRAPH_PREFIX "%d %d %d %d %d %s" TURAG_DEBUG_NEWLINE,
						   index, x_left, y_bottom, width, height, title);
		++channels;
	}
}




void GraphFixedTime::init(const char *name, unsigned time, std::initializer_list<const char*> channelNames, bool enabled) {
	Graph::init(name, enabled);

	for (const char* channel_title : channelNames) {
		addChannel(channel_title, time);
	}
}

void GraphFixedTime::init(const char *name, unsigned time, std::initializer_list<std::initializer_list<const char*>> channelNames, std::initializer_list<const char*> channelGroups, bool enabled) {
	Graph::init(name, enabled);

#if __cplusplus >= 201300L
	static_assert(channelNames.size() == channelGroups.size(), "channelNames.size() == channelGroups.size() not true");
#else
	if (channelNames.size() != channelGroups.size()) {
        turag_internal_error("GraphAuto::GraphAuto: channelNames.size() == channelGroups.size() not true");
		return;
	}
#endif

	auto groupIterator = channelGroups.begin();

	for (const auto&list : channelNames) {
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
	if (!isInitialized()) {
        turag_internal_error("tried to call GraphFixedTime::addChannel prior to initialization");
	} else {
		turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_GRAPH_CHANNEL TURAG_DEBUG_GRAPH_PREFIX "%d %d %s" TURAG_DEBUG_NEWLINE,
						   index, time, title);
		++channels;
	}
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


