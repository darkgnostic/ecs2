#pragma once
#include <cassert>
#include <memory>
#include <vector>
#include <iostream>
#include "events/Event.h"
#include "events/EventResult.h"

namespace wcs {

	struct Event;
	struct Entity;
	struct Component;

	using SharedEntity = std::shared_ptr<Entity>;
	using SharedEvent = std::shared_ptr<Event>;
	using WeakEntity = std::weak_ptr<Entity>;
	using WeakComponent = std::weak_ptr<Component>;
	
	struct Component
	{
		WeakEntity parent;
		inline static bool dump = true;
		
		virtual ~Component() = default;
		virtual void Dump() {}

		virtual void OnEvent(SharedEvent& event, EventResult& event_result) { }
		virtual std::string description() const { return ""; }

		virtual void OnEvent(SharedEvent&& event) { EventResult er; OnEvent(event, er); }
		
		template<typename T>
		std::shared_ptr<T> GetParent() const
		{
			const auto p = parent.lock();
			const auto entity = std::dynamic_pointer_cast<T>(p);
			assert( p != nullptr );
			return entity;
		}
	};

	struct Entity : std::enable_shared_from_this<Entity>
	{
		virtual ~Entity() = default;
		
		std::vector<std::shared_ptr<Component>> component;

		template<typename T, typename... Args>
		std::shared_ptr<T> add(Args&&... args)
		{
 			component.push_back(std::make_shared<T>(std::forward<Args>(args)...));
			SetParent(component.back());
			
			return std::dynamic_pointer_cast<T>(component.back());
		}

		void SetParent(std::shared_ptr<Component> c)
		{
			c->parent=weak_from_this();
		}

		template<typename T>
		T* get()
		{
			for (auto& t : component) {
				if (auto* cast = dynamic_cast<T*>(t.get()))
					return cast;
			}
			return nullptr;
		}

		template<typename T>
		std::vector<std::shared_ptr<T>> get_all()
		{
			std::vector<std::shared_ptr<T>> com;
			for (auto& t : component) {
				if (auto* cast = dynamic_cast<T*>(t.get()))
					com.push_back(std::dynamic_pointer_cast<T>(t));
			}
			return com;
		}
		
		EventResult OnEvent(const SharedEvent& event)
		{
			EventResult event_result;
			EventProcess(event, event_result);
			return event_result;
		}

		void OnEvent(const SharedEvent& event, EventResult& event_result)
		{
			EventProcess(event, event_result);
		}

		template<typename T>
		void Dump()
		{
			if (T* t = get<T>())
				t->Dump();
		}
	private:
		void EventProcess(SharedEvent event, EventResult& event_result)
		{
			for (auto& t : component) {
				t->OnEvent(event, event_result);
				if (event_result.result == EventProcess::kFinish ) {
					break;
				}
			}
		}
		
	};
};