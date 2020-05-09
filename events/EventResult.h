#pragma once

namespace wcs
{
	enum class EventProcess
	{
		kContinue,
		kFinish,
		kFinishAndContinue
	};
	
	struct EventResult
	{
		EventProcess result = EventProcess::kContinue;
		int operation_result = 0;
		EventResult() - default;

		EventResult(EventResult& e) : result(e.result), operation_result(e.operation_result)
		{
			result = e.result;
		}
		EventResult(EventResult&& e) noexcept : result(e.result), operation_result(e.operation_result)
		{
			result = e.result;
		}
		~EventResult()
		{
		}

	};
}