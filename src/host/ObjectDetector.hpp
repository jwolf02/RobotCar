#ifndef __OBJECTDETECTOR_HPP
#define __OBJECTDETECTOR_HPP

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>

/***
 * Prediction class that is outputted by the
 * object detector
 */
class Prediction {
public:
    Prediction() = default;

    Prediction(int id, int left, int top, int width, int height, float confidence, const cv::String &name="") :
            class_id(id), offset(left, top), size(width, height), confidence(confidence), class_name(name){}

    int class_id = -1;

    cv::Point offset = cv::Point(-1, -1);

    cv::Size size = cv::Size(0, 0);

    float confidence = -1.0;

    cv::String class_name;

};

void drawPredictions(cv::Mat &frame, const std::vector<Prediction> &predictions, bool drawLabels=true,
                        const cv::Scalar &color=cv::Scalar(0, 0, 255), int thickness=1, int lineType=cv::LINE_8, int shift=0);

/***
 * Wrapper class around OpenCV's DNN module to support
 * object detection models.
 * It has been tested with SSD-Mobilenet v2 and Tiny Yolo v3.
 */
class ObjectDetector {
public:

    /// default constructor
    ObjectDetector() = default;

    /***
     * read the getNetwork from the specified files
     * @param model file containing the getNetwork parameters
     * @param config file containing the getNetwork architecture
     * @param framework the framwork that was used to make the mode
     */
    explicit ObjectDetector(const cv::String &model, const cv::String &config="", const cv::String &framework="");

    // basically what is called by the constructor above
    void readNetwork(const cv::String &model, const cv::String &config="", const cv::String &framework="");

    /**
     * set the preferred getBackend
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

    void setCrop(bool crop);

    void setType(int type);

    void setNMSThreshold(float nmsThreshold);

    void setConfidenceThreshold(float confThreshold);

    void setClasses(const std::vector<std::string> &classes);

    void setDetectableClasses(unsigned num_classes);

    /***
     * before running the getNetwork this function must be called,
     * as this gets the output tensors and is dependent on the
     * getBackend and the getNetwork
     */
    void getOutputLayers();

    /***
     * run detector on the input frame, based on the parameters
     * a list of Predictions of  objects in the image
     * @param frame image
     * @param drawPred if true the prediction are drawn as labelled bounding boxes on the image
     * @param drawLabel if predictions are drawn, then this indicated it the label should be drawn to the prediction
     * @return
     */
    std::vector<Prediction> run(cv::Mat &frame, bool drawPred=false, bool drawLabel=true);

    /***
     * getters for various parameters
     */
    cv::dnn::Backend getBackend() const;

    cv::dnn::Target getTarget() const;

    const cv::dnn::Net& getNetwork() const;

    float getConfidenceThreshold() const;

    float getNMSThreshold() const;

    float getFPS() const;

    float getLatency() const;

    std::vector<cv::dnn::MatShape> getInputShapes() const;

private:

    // preprocess the frame before running the detector
    void preprocess(const cv::Mat &frame);

    // postprocess the output tensors from the detector to create
    // a list of predictions
    void postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs, std::vector<Prediction> &pred);

    cv::dnn::Backend _backend = cv::dnn::DNN_BACKEND_DEFAULT;

    cv::dnn::Target _target = cv::dnn::DNN_TARGET_CPU;

    // the names of the output tensors
    std::vector<cv::String> _out_names;

    // class label lookup table
    std::vector<std::string> _classes;

    cv::dnn::Net _net;

    cv::Size _size = cv::Size(0, 0);

    double _scale = 1.0;

    // mean to be subtracted from the image
    cv::Scalar _mean = cv::Scalar();

    // threshold for non maximum suppression
    float _nms_threshold = 0.4;

    // confidence threshold
    float _conf_threshold = 0.5;

    float _fps = 0.0;

    float _latency = 0.0;

    unsigned _num_classes = 1000000;

    int _type = CV_8U;

    bool _swap_rb = true;

    bool _crop = false;

};

#endif // __OBJECTDETECTOR_HPP
