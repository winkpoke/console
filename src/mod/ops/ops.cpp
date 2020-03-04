#include "cl.h"
#include "ops.h"
#include <librealsense2/rs.hpp>

namespace mod::ops::control {
    bool init(ops_t* p)
    {
        assert(p);
        if (p) {
            new(&p->pipeline)rs2::pipeline;
            new(&p->runnable)std::atomic_flag;
            p->runnable.clear();
        }
        return true;
    }

    void drop(ops_t* p)
    {
        assert(p);
        if (p) {
            
            if (p->thread) {
                p->runnable.clear();
                p->thread->join();
                delete p->thread;
            }
            p->pipeline.~pipeline();
            free(p);
        }
    }


    void start(ops_t* ops)
    {
        assert(ops);
        rs2::pipeline& p = ops->pipeline;
        p.start();
        ops->thread = new std::thread(capture, ops);
    }

    void capture(ops_t* ops)
    {
        assert(ops);
        rs2::pipeline& p = ops->pipeline;
        ops->runnable.test_and_set();
        ops->runnable.clear();
        while (ops->runnable.test_and_set())
        {
            // Block program until frames arrive
            rs2::frameset frames = p.wait_for_frames();
            rs2::pointcloud pc;
            rs2::points ps = pc.calculate(frames.get_depth_frame());

            // Try to get a frame of a depth image
            rs2::depth_frame depth = frames.get_depth_frame();

            // Get the depth frame's dimensions
            float width = depth.get_width();
            float height = depth.get_height();

            // Query the distance from the camera to the object in the center of the image
            float dist_to_center = depth.get_distance(width / 2, height / 2);

            // Print the distance
            std::cout << "The camera is facing an object " << dist_to_center << " meters away \r";
        }
    }
}
