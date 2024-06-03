// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include <stack>

namespace origin
{
	template<typename T>
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
		virtual void Undo() = 0;
	};

	template <typename T>
	class FunctionCommand : public Command<T>
	{
	public:
		FunctionCommand(T &target, std::function<void()> executeFn, std::function<void()> undoFn)
			: m_Target(target), m_ExecuteFn(executeFn), m_UndoFn(undoFn)
		{ }

		void Execute() override
		{
			m_ExecuteFn();
		}

		void Undo() override
		{
			m_UndoFn();
		}

	private:
		T &m_Target;
		std::function<void()> m_ExecuteFn;
		std::function<void()> m_UndoFn;
	};

	template <typename T>
	class CommandManager
	{
	public:
		void executeCommand(std::unique_ptr<Command<T>> command)
		{
			command->Execute();
			m_UndoStack.push(std::move(command));

			while (!m_RedoStack.empty()) m_RedoStack.pop();
		}

		void undo()
		{
			if (!m_UndoStack.empty())
			{
				auto command = std::move(m_UndoStack.top());
				m_UndoStack.pop();
				command->Undo();
				m_RedoStack.push(std::move(command));
			}
			else OGN_CORE_WARN("[CommandManager] No actions to redo")
		}

		void redo()
		{
			if (!m_RedoStack.empty())
			{
				auto command = std::move(m_RedoStack.top());
				m_RedoStack.pop();
				command->Execute();
				m_UndoStack.push(std::move(command));
			}
			else OGN_CORE_WARN("[CommandManager] No actions to redo")
		}

	private:
		std::stack<std::unique_ptr<Command<T>>> m_UndoStack;
		std::stack<std::unique_ptr<Command<T>>> m_RedoStack;
	};
}
