//
// Copyright © 2020, 2023 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "ActivationTestHelper.hpp"

#include <armnn_delegate.hpp>

#include <flatbuffers/flatbuffers.h>
#include <tensorflow/lite/interpreter.h>
#include <schema_generated.h>

#include <doctest/doctest.h>

namespace armnnDelegate
{


void ActivationReLuTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    // Calculate output values for input.
    auto f = [](float value)
    {
        return std::fmax(0.0f, value);
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_RELU,
                   backends,
                   inputData,
                   outputExpectedData);
}

void ActivationBoundedReluTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    const float a = 6.0f;
    const float b = 0.0f;
    // Calculate output values for input.
    auto f = [a, b](float value)
    {
        return std::min(a, std::max(b, value));
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_RELU6,
                   backends,
                   inputData,
                   outputExpectedData);
}

void ActivationSigmoidTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    // Calculate output values for input.
    auto f = [](float value)
    {
        return 1.0f / (1.0f + std::exp(-value));
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_LOGISTIC,
                   backends,
                   inputData,
                   outputExpectedData);
}


void ActivationTanHTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    // Calculate output values for input.
    auto f = [](float value)
    {
        return tanhf(value);
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_TANH,
                   backends,
                   inputData,
                   outputExpectedData);
}

void ActivationEluTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    // Calculate output values for input.
    auto f = [](float value)
    {
        if (value < 0)
        {
            // alpha * (exp(x) - 1)
            return 1 * (std::exp(value) - 1);
        }
        return value;
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_ELU,
                   backends,
                   inputData,
                   outputExpectedData);
}

void ActivationHardSwishTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    // Calculate output values for input.
    auto f = [](float x)
    {
        // Break down the calculation to help with verification.
        // hard_swish(x) = x * relu6(x+3) / 6
        // relu6(x) = min(max(x,0),6)
        float reLu6_step1 = std::max((x + 3),0.0f);
        float reLu6Complete = std::min(reLu6_step1, 6.0f);
        float hardSwish_step1 = x * reLu6Complete;
        float result = hardSwish_step1 / 6;
        return result;
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_HARD_SWISH,
                   backends,
                   inputData,
                   outputExpectedData);
}

void ActivationLeakyReLuTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData = {
            -0.1f, -0.2f, -0.3f, -0.4f,
            0.1f,  0.2f,  0.3f,  0.4f,
            -1.0f, -2.0f, -3.0f, -4.0f,
            1.0f,  2.0f,  3.0f,  4.0f
    };

    float alpha = 0.3f;

    // Calculate output values for input.
    auto f = [alpha](float value)
    {
        return value > 0 ? value : value * alpha;
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_LEAKY_RELU,
                   backends,
                   inputData,
                   outputExpectedData,
                   alpha);
}

void ActivationGeluTest(std::vector<armnn::BackendId>& backends)
{
    std::vector<float> inputData =
    {
        -0.1f, -0.2f, -0.3f, -0.4f,
         0.1f,  0.2f,  0.3f,  0.4f,
        -1.0f, -2.0f, -3.0f, -4.0f,
         1.0f,  2.0f,  3.0f,  4.0f
    };

    // Calculate output values for input.
    auto f = [](float x)
    {
        // gelu(x) = x * 1/2 * (1 + erf(x / sqrt(2))),
        // where erf is Gaussian error function
        auto result = x * (0.5f * (1.0f + erff(static_cast<float>(x / std::sqrt(2)))));
        return result;
    };
    std::vector<float> outputExpectedData(inputData.size());
    std::transform(inputData.begin(), inputData.end(), outputExpectedData.begin(), f);

    ActivationTest(tflite::BuiltinOperator_GELU,
                   backends,
                   inputData,
                   outputExpectedData);
}

TEST_SUITE("Activation_CpuRefTests")
{

TEST_CASE ("Activation_ReLu_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationReLuTest(backends);
}

TEST_CASE ("Activation_Bounded_Relu6_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationBoundedReluTest(backends);
}

TEST_CASE ("Activation_Sigmoid_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationSigmoidTest(backends);
}

TEST_CASE ("Activation_TanH_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationTanHTest(backends);
}

TEST_CASE ("Activation_Elu_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationEluTest(backends);
}

TEST_CASE ("Activation_HardSwish_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationHardSwishTest(backends);
}

TEST_CASE ("Activation_LeakyRelu_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationLeakyReLuTest(backends);
}

TEST_CASE ("Activation_Gelu_CpuRef_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuRef };
    ActivationGeluTest(backends);
}

}

TEST_SUITE("Activation_CpuAccTests")
{

TEST_CASE ("Activation_ReLu_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationReLuTest(backends);
}

TEST_CASE ("Activation_Bounded_Relu6_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationBoundedReluTest(backends);
}

TEST_CASE ("Activation_Sigmoid_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationSigmoidTest(backends);
}

TEST_CASE ("Activation_TanH_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationTanHTest(backends);
}

TEST_CASE ("Activation_Elu_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationEluTest(backends);
}

TEST_CASE ("Activation_HardSwish_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationHardSwishTest(backends);
}

TEST_CASE ("Activation_LeakyRelu_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationLeakyReLuTest(backends);
}

TEST_CASE ("Activation_Gelu_CpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::CpuAcc };
    ActivationGeluTest(backends);
}

}

TEST_SUITE("Activation_GpuAccTests")
{

TEST_CASE ("Activation_ReLu_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationReLuTest(backends);
}

TEST_CASE ("Activation_Bounded_Relu6_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationBoundedReluTest(backends);
}

TEST_CASE ("Activation_Sigmoid_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationSigmoidTest(backends);
}

TEST_CASE ("Activation_TanH_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationTanHTest(backends);
}

TEST_CASE ("Activation_Elu_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationEluTest(backends);
}

TEST_CASE ("Activation_HardSwish_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationHardSwishTest(backends);
}

TEST_CASE ("Activation_LeakyRelu_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationLeakyReLuTest(backends);
}

TEST_CASE ("Activation_Gelu_GpuAcc_Test")
{
    std::vector<armnn::BackendId> backends = { armnn::Compute::GpuAcc };
    ActivationGeluTest(backends);
}

}

} // namespace armnnDelegate