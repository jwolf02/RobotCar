#include <ObjectDetector.hpp>
#include <opencv2/imgproc.hpp>
#include <stdexcept>

static void drawPrediction(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame, const std::vector<std::string> &classes) {
    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 255, 0));

    const std::string label = classId < classes.size() ? classes[classId] : cv::format("%.2f", conf);

    int baseLine;
    cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    top = std::max(top, labelSize.height);
    cv::rectangle(frame, cv::Point(left, top - labelSize.height),
              cv::Point(left + labelSize.width, top + baseLine), cv::Scalar::all(255), cv::FILLED);
    cv::putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar());
}

void ObjectDetector::drawPredictions(cv::Mat &frame, const std::vector<Prediction> &predictions, const std::vector<std::string> &classes) {
    for (const auto &pred : predictions) {
        drawPrediction(pred.class_id, pred.confidence, pred.left, pred.top, pred.left + pred.width, pred.top + pred.height, frame, classes);
    }
}

ObjectDetector::ObjectDetector(const std::string &model, const std::string &config, const std::string &framework) {
    readNetwork(model, config, framework);
}

void ObjectDetector::readNetwork(const std::string & model, const std::string & config, const std::string & framework) {
    _net = cv::dnn::readNet(model, config, framework);
}

void ObjectDetector::setBackend(cv::dnn::Backend backend) {
    _backend = backend;
    _net.setPreferableBackend(backend);
}

void ObjectDetector::setTarget(cv::dnn::Target target) {
    _target = target;
    _net.setPreferableTarget(target);
}

void ObjectDetector::setScale(double scale) {
    _scale = scale;
}

void ObjectDetector::setMean(const cv::Scalar &mean) {
    _mean = mean;
}

void ObjectDetector::setSwapRB(bool swap) {
    _swap_rb = swap;
}

void ObjectDetector::setSize(const cv::Size &size) {
    _size = size;
}

void ObjectDetector::setType(int type) {
    _type = type;
}

void ObjectDetector::setNMSThreshold(double nmsThreshold) {
    _nms_threshold = nmsThreshold;
}

void ObjectDetector::setConfidenceThreshold(double confThreshold) {
    _conf_threshold = confThreshold;
}

void ObjectDetector::setClasses(const std::vector<std::string> &classes) {
    _classes = classes;
}

void ObjectDetector::setDetectableClasses(unsigned int num_classes) {
    _num_classes = num_classes;
}

void ObjectDetector::getOutputLayers() {
    _out_names = _net.getUnconnectedOutLayersNames();
}

std::vector<ObjectDetector::Prediction> ObjectDetector::run(cv::Mat &frame, bool drawPred) {
    const std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
    preprocess(frame);
    std::vector<cv::Mat> outs;
    _net.forward(outs, _out_names);
    auto tmp = postprocess(frame, outs, drawPred);
    if (drawPred) {
        drawPredictions(frame, tmp, _classes);
    }
    _latency = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin).count());
    _fps = 1.0 / _latency;
    return tmp;
}

cv::dnn::Target ObjectDetector::target() const {
    return _target;
}

cv::dnn::Backend ObjectDetector::backend() const {
    return _backend;
}

const cv::dnn::Net &ObjectDetector::network() const {
    return _net;
}

double ObjectDetector::confidenceThreshold() const {
    return _conf_threshold;
}

double ObjectDetector::nmsThreshold() const {
    return _nms_threshold;
}

double ObjectDetector::getFPS() const {
    return _fps;
}

double ObjectDetector::getLatency() const {
    return _latency;
}

void ObjectDetector::preprocess(const cv::Mat &frame) {
    static cv::Mat blob;
    static const cv::Size size(_size.width == 0 ? frame.cols : _size.width, _size.height == 0 ? frame.rows : _size.height);
    cv::dnn::blobFromImage(frame, blob, 1.0, size, _mean, _swap_rb, false, _type);

    // Run a model
    _net.setInput(blob, "", _scale, _mean);
    // Faster-RCNN or R-FCN
    if (_net.getLayer(0)->outputNameToIndex("im_info") != -1) {
        cv::resize(frame, frame, size);
        cv::Mat imInfo = (cv::Mat_<float>(1, 3) << size.height, size.width, 1.6f);
        _net.setInput(imInfo, "im_info");
    }
}

std::vector<ObjectDetector::Prediction> ObjectDetector::postprocess(cv::Mat &frame, const std::vector<cv::Mat> &outs, bool drawPred) {
    static std::vector<int> outLayers = _net.getUnconnectedOutLayers();
    static std::string outLayerType = _net.getLayer(outLayers[0])->type;

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    if (outLayerType == "DetectionOutput") {
        // Network produces output blob with a shape 1x1xNx7 where N is a number of
        // detections and an every detection is a vector of values
        // [batchId, classId, confidence, left, top, right, bottom]
        for (auto &out : outs) {
            auto data = (float *) out.data;
            for (size_t i = 0; i < out.total(); i += 7) {
                float confidence = data[i + 2];
                if (confidence > _conf_threshold) {
                    int left   = (int) data[i + 3];
                    int top    = (int) data[i + 4];
                    int right  = (int) data[i + 5];
                    int bottom = (int) data[i + 6];
                    int width  = right - left + 1;
                    int height = bottom - top + 1;
                    if (width <= 2 || height <= 2) {
                        left   = (int) (data[i + 3] * frame.cols);
                        top    = (int) (data[i + 4] * frame.rows);
                        right  = (int) (data[i + 5] * frame.cols);
                        bottom = (int) (data[i + 6] * frame.rows);
                        width  = right - left + 1;
                        height = bottom - top + 1;
                    }
                    // Skip 0th background class id.
                    classIds.push_back((int) (data[i + 1]) - 1);
                    boxes.emplace_back(cv::Rect(left, top, width, height));
                    confidences.push_back(confidence);
                }
            }
        }
    } else if (outLayerType == "Region") {
        for (auto &out : outs) {
            // Network produces output blob with a shape NxC where N is a number of
            // detected objects and C is a number of classes + 4 where the first 4
            // numbers are [center_x, center_y, width, height]
            auto data = (float *) out.data;
            for (int j = 0; j < out.rows; ++j, data += out.cols) {
                cv::Mat scores = out.row(j).colRange(5, out.cols);
                cv::Point classIdPoint;
                double confidence;
                cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > _conf_threshold) {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.emplace_back(cv::Rect(left, top, width, height));
                }
            }
        }
    } else {
        CV_Error(cv::Error::StsNotImplemented, "unknown output layer type: " + outLayerType);
    }

    // non maximum suppression
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, _conf_threshold, _nms_threshold, indices);

    // filter for prediction sufficing NMS threshold
    std::vector<Prediction> pred(indices.size());
    std::for_each(indices.begin(), indices.end(), [&](int idx) {
        if (classIds[idx] < _num_classes)
            pred.emplace_back(Prediction(classIds[idx], boxes[idx].x, boxes[idx].y, boxes[idx].width,
                                                                    boxes[idx].height, confidences[idx])); });

    // draw predictions
    if (drawPred) {
        for (auto idx : indices) {
            const cv::Rect box = boxes[idx];
            drawPrediction(classIds[idx], confidences[idx], box.x, box.y,
                           box.x + box.width, box.y + box.height, frame, _classes);
        }
    }
    return pred;
}
