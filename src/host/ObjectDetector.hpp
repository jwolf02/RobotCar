#ifndef __OBJECTDETECTOR_HPP
#define __OBJECTDETECTOR_HPP

#include <opencv2/dnn.hpp>

class ObjectDetector {
public:

    struct Prediction {

        Prediction() = default;

        Prediction(int id, int left, int top, int width, int height, double confidence) :
            class_id(id), left(left), top(top), width(width), height(height), confidence(confidence) {}

        int class_id = -1;

        int left = -1;

        int top = -1;

        int width = -1;

        int height = -1;

        double confidence = -1.0;
    };

    static void drawPredictions(cv::Mat &frame, const std::vector<Prediction> &predictions, const std::vector<std::string> &classes);

    ObjectDetector() = default;

    ObjectDetector(const std::string &model, const std::string &config="", const std::string &framework="");

    void readNetwork(const std::string &model, const std::string &config="", const std::string &framework="");

    void setBackend(cv::dnn::Backend backend);

    void setTarget(cv::dnn::Target target);

    void setScale(double scale);

    void setSize(const cv::Size &size);

    void setMean(const cv::Scalar &mean);

    void setSwapRB(bool swap);

    void setType(int type);

    void setNMSThreshold(double nmsThreshold);

    void setConfidenceThreshold(double confThreshold);

    void setClasses(const std::vector<std::string> &classes);

    void setDetectableClasses(unsigned num_classes);

    void getOutputLayers();

    std::vector<Prediction> run(cv::Mat &frame, bool drawPred=false);

    void operator()(cv::Mat &frame) {
        run(frame, true);
    }

    cv::dnn::Backend backend() const;

    cv::dnn::Target target() const;

    const cv::dnn::Net& network() const;

    double confidenceThreshold() const;

    double nmsThreshold() const;

    double getFPS() const;

    double getLatency() const;

private:

    void preprocess(const cv::Mat &frame);

    std::vector<Prediction> postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs, bool drawPred);

    cv::dnn::Backend _backend = cv::dnn::DNN_BACKEND_DEFAULT;

    cv::dnn::Target _target = cv::dnn::DNN_TARGET_CPU;

    std::vector<cv::String> _out_names;

    std::vector<std::string> _classes;

    cv::dnn::Net _net;

    cv::Size _size = cv::Size(0, 0);

    double _scale = 0.00392;

    cv::Scalar _mean = { 0.0, 0.0, 0.0, 0.0 };

    double _nms_threshold = 0.4;

    double _conf_threshold = 0.5;

    double _fps = 0.0;

    double _latency = 0.0;

    unsigned _num_classes = 1000000;

    int _type = CV_8U;

    bool _swap_rb = true;

};

#endif // __OBJECTDETECTOR_HPP
