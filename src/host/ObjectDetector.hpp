#ifndef __OBJECTDETECTOR_HPP
#define __OBJECTDETECTOR_HPP

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

/***
 * Wrapper class around OpenCV's DNN module to support
 * object detection models.
 * It has been tested with SSD-Mobilenet v2 and Tiny Yolo v3.
 */
class ObjectDetector {
public:

    /***
     * Prediction class that is outputted by the
     * object detector
     */
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

    /***
     * draw predicted bounding boxes, confidence scores and label on the frame
     * @param frame
     * @param predictions
     * @param classes
     */
    static void drawPredictions(cv::Mat &frame, const std::vector<Prediction> &predictions, const std::vector<std::string> &classes);

    ObjectDetector() = default;

    /***
     * read the network from the specified files
     * @param model file containing the network parameters
     * @param config file containing the network architecture
     * @param framework the framwork that was used to make the mode
     */
    ObjectDetector(const std::string &model, const std::string &config="", const std::string &framework="");

    // basically what is called by the contructor above
    void readNetwork(const std::string &model, const std::string &config="", const std::string &framework="");

    /**
     * set the preferred backend
     * @param backend
     */
    void setBackend(cv::dnn::Backend backend);

    /***
     * set the preferred targer
     * @param target
     */
    void setTarget(cv::dnn::Target target);

    /***
     * setters for various parameters
     */
    void setScale(double scale);

    void setSize(const cv::Size &size);

    void setMean(const cv::Scalar &mean);

    void setSwapRB(bool swap);

    void setType(int type);

    void setNMSThreshold(double nmsThreshold);

    void setConfidenceThreshold(double confThreshold);

    void setClasses(const std::vector<std::string> &classes);

    void setDetectableClasses(unsigned num_classes);

    /***
     * before running the network this function must be called,
     * as this gets the output tensors and is dependent on the
     * backend and the network
     */
    void getOutputLayers();

    /***
     * run detector on the input frame, based on the parameters
     * a list of Predictions of  objects in the image
     * @param frame image
     * @param drawPred if true the prediction are drawn as labelled bounding boxes on the image
     * @return
     */
    std::vector<Prediction> run(cv::Mat &frame, bool drawPred=false);

    /***
     * wrapped around run, drawing the bboxes and not returning a list of predictions
     * @param frame
     */
    void operator()(cv::Mat &frame) {
        run(frame, true);
    }

    /***
     * getters for various parameters
     */
    cv::dnn::Backend backend() const;

    cv::dnn::Target target() const;

    const cv::dnn::Net& network() const;

    double confidenceThreshold() const;

    double nmsThreshold() const;

    double getFPS() const;

    double getLatency() const;

private:

    // preprocess the frame before running the detector
    void preprocess(const cv::Mat &frame);

    // postprocess the output tensors from the detector to create
    // a list of predictions
    std::vector<Prediction> postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs, bool drawPred);

    cv::dnn::Backend _backend = cv::dnn::DNN_BACKEND_DEFAULT;

    cv::dnn::Target _target = cv::dnn::DNN_TARGET_CPU;

    // the names of the output tensors
    std::vector<cv::String> _out_names;

    // class label lookup table
    std::vector<std::string> _classes;

    cv::dnn::Net _net;

    cv::Size _size = cv::Size(0, 0);

    double _scale = 0.00392;

    // mean to be subtracted from the image
    cv::Scalar _mean = { 0.0, 0.0, 0.0, 0.0 };

    // threshold for non maximum suppression
    double _nms_threshold = 0.4;

    // confidence threshold
    double _conf_threshold = 0.5;

    double _fps = 0.0;

    double _latency = 0.0;

    unsigned _num_classes = 1000000;

    int _type = CV_8U;

    bool _swap_rb = true;

};

#endif // __OBJECTDETECTOR_HPP
