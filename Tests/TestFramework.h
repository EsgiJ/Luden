#pragma once
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <iomanip>

namespace Luden::Testing
{
	class Timer
	{
	public:
		Timer() { Reset(); }

		void Reset()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		float ElapsedMillis() const
		{
			auto now = std::chrono::high_resolution_clock::now();
			return std::chrono::duration<float, std::milli>(now - m_Start).count();
		}

		float ElapsedSeconds() const
		{
			return ElapsedMillis() / 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

	struct TestResult
	{
		std::string Name;
		bool Passed = false;
		std::string Message;
		float DurationMs = 0.0f;
	};

	struct BenchmarkResult
	{
		std::string Name;
		float AverageMs = 0.0f;
		float MinMs = 0.0f;
		float MaxMs = 0.0f;
		int Iterations = 0;
	};

	class TestCase
	{
	public:
		using TestFunc = std::function<void()>;

		TestCase(const std::string& name, TestFunc func)
			: m_Name(name), m_Function(func) {
		}

		TestResult Run()
		{
			TestResult result;
			result.Name = m_Name;

			Timer timer;
			try
			{
				m_Function();
				result.Passed = true;
				result.Message = "PASSED";
			}
			catch (const std::exception& e)
			{
				result.Passed = false;
				result.Message = std::string("FAILED: ") + e.what();
			}
			catch (...)
			{
				result.Passed = false;
				result.Message = "FAILED: Unknown exception";
			}

			result.DurationMs = timer.ElapsedMillis();
			return result;
		}

		const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
		TestFunc m_Function;
	};

	class BenchmarkCase
	{
	public:
		using BenchFunc = std::function<void()>;

		BenchmarkCase(const std::string& name, BenchFunc func, int iterations = 100)
			: m_Name(name), m_Function(func), m_Iterations(iterations) {
		}

		BenchmarkResult Run()
		{
			BenchmarkResult result;
			result.Name = m_Name;
			result.Iterations = m_Iterations;

			float totalTime = 0.0f;
			float minTime = FLT_MAX;
			float maxTime = 0.0f;

			m_Function();

			for (int i = 0; i < m_Iterations; i++)
			{
				Timer timer;
				m_Function();
				float elapsed = timer.ElapsedMillis();

				totalTime += elapsed;
				minTime = std::min(minTime, elapsed);
				maxTime = std::max(maxTime, elapsed);
			}

			result.AverageMs = totalTime / m_Iterations;
			result.MinMs = minTime;
			result.MaxMs = maxTime;

			return result;
		}

		const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
		BenchFunc m_Function;
		int m_Iterations;
	};

	class TestSuite
	{
	public:
		TestSuite(const std::string& name) : m_Name(name) {}

		void AddTest(const std::string& name, TestCase::TestFunc func)
		{
			m_Tests.emplace_back(name, func);
		}

		void AddBenchmark(const std::string& name, BenchmarkCase::BenchFunc func, int iterations = 100)
		{
			m_Benchmarks.emplace_back(name, func, iterations);
		}

		void RunTests()
		{
			std::cout << "\n========================================\n";
			std::cout << "Running Test Suite: " << m_Name << "\n";
			std::cout << "========================================\n\n";

			int passed = 0;
			int failed = 0;

			for (auto& test : m_Tests)
			{
				auto result = test.Run();

				if (result.Passed)
				{
					std::cout << "[" << (char)251 << "] ";
					passed++;
				}
				else
				{
					std::cout << "[x] ";
					failed++;
				}

				std::cout << result.Name << " (" << std::fixed << std::setprecision(2)
					<< result.DurationMs << " ms) - " << result.Message << "\n";
			}

			std::cout << "\n----------------------------------------\n";
			std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
			std::cout << "========================================\n";
		}

		void RunBenchmarks()
		{
			if (m_Benchmarks.empty())
				return;

			std::cout << "\n========================================\n";
			std::cout << "Running Benchmarks: " << m_Name << "\n";
			std::cout << "========================================\n\n";

			for (auto& benchmark : m_Benchmarks)
			{
				std::cout << "Running: " << benchmark.GetName() << "...\n";
				auto result = benchmark.Run();

				std::cout << "  Average: " << std::fixed << std::setprecision(3)
					<< result.AverageMs << " ms\n";
				std::cout << "  Min:     " << result.MinMs << " ms\n";
				std::cout << "  Max:     " << result.MaxMs << " ms\n";
				std::cout << "  Iterations: " << result.Iterations << "\n\n";
			}

			std::cout << "========================================\n";
		}

	private:
		std::string m_Name;
		std::vector<TestCase> m_Tests;
		std::vector<BenchmarkCase> m_Benchmarks;
	};

	inline void Assert(bool condition, const std::string& message = "Assertion failed")
	{
		if (!condition)
			throw std::runtime_error(message);
	}

	inline void AssertEqual(int a, int b, const std::string& message = "")
	{
		if (a != b)
		{
			throw std::runtime_error(message.empty()
				? "Expected " + std::to_string(a) + " == " + std::to_string(b)
				: message);
		}
	}

	inline void AssertEqual(float a, float b, float epsilon = 0.0001f, const std::string& message = "")
	{
		if (std::abs(a - b) > epsilon)
		{
			throw std::runtime_error(message.empty()
				? "Expected " + std::to_string(a) + " == " + std::to_string(b)
				: message);
		}
	}

	inline void AssertNotNull(void* ptr, const std::string& message = "Pointer is null")
	{
		if (ptr == nullptr)
			throw std::runtime_error(message);
	}

	inline void AssertTrue(bool condition, const std::string& message = "Expected true")
	{
		if (!condition)
			throw std::runtime_error(message);
	}

	inline void AssertFalse(bool condition, const std::string& message = "Expected false")
	{
		if (condition)
			throw std::runtime_error(message);
	}
}