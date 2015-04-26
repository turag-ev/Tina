#ifndef TINAPP_DEBUG_GRAPH_H
#define TINAPP_DEBUG_GRAPH_H

#include <tina/debug/graph.h>
#include <tina++/math.h>
#include <tina++/debug.h>

#include <initializer_list>


namespace TURAG {
namespace Debug {

class Graph {
public:
    Graph(const char* name) {
        index = turag_graph_create(name);
        channels = 0;
	}

    Graph(const char *name, std::initializer_list<const char*> channels) {
        index = turag_graph_create(name);
        channels = 0;

        for (const char* channel_title : channels) {
            turag_graph_add_channel(channels, channel_title);
            ++channels;
        }
    }

    void addChannel(const char* title) {
        turag_graph_add_channel(index, title);
        ++channels;
    }

    void addChannelFixedTime(const char* title, unsigned time) {
        turag_graph_add_channel_fixed_time(index, title, time);
        ++channels;
    }

    void addChannelFixedXY(const char* title, int x_left, int y_bottom, unsigned width, unsigned height) {
        turag_graph_add_channel_fixed_xy(index, title, x_left, y_bottom, width, height);
        ++channels;
    }

    template <size_t N>
    void plotData(float time, float& data[N]) {
        if (N > channels) {
            turag_errorf("Graph::plotData: got %d elements, but only %d channels are defined", N, channels);
        } else {
            turag_graph_data(index, time, data, N);
        }
    }

    void plotData(float time, float* data, size_t count) {
        if (count > channels) {
            turag_errorf("Graph::plotData: got %d elements, but only %d channels are defined", count, channels);
        } else {
            turag_graph_data(index, time, data, N);
        }
    }

    void plot2D(float& data[N]) {
        static_assert(N % 2 == 0, "Graph::plot2D: given array must be of even length");

        if (N / 2 > channels) {
            turag_errorf("Graph::plotData: got elements for %d channels, but only %d channels are defined", N / 2, channels);
        } else {
            turag_graph_data2d(index, data, N);
        }
    }

    void plot2D(float* data, size_t count) {
        if (count > channels) {
            turag_errorf("Graph::plotData: got elements for %d channels, but only %d channels are defined", count, channels);
        } else {
            turag_graph_data2d(index, data, count);
        }
    }

//    void addChannelGroup(const char* label, )

private:
    unsigned index;
    unsigned channels;
};

} // namespace Debug
} // namespace TURAG

#endif // TINAPP_DEBUG_GRAPH_H
