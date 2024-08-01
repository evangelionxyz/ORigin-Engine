// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "ConsoleManager.h"
#include "Origin/Scene/Components/Components.h"

#include <stack>

namespace origin
{

	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
		virtual void Undo() = 0;
	};

	class CommandManager
	{
	public:
		CommandManager() = default;
		static CommandManager &Instance()
		{
			static CommandManager s_Instance;
			return s_Instance;
		}

		void ExecuteCommand(std::unique_ptr<Command> command)
		{
			command->Execute();
			m_UndoStack.push(std::move(command));
			m_RedoStack = std::stack<std::unique_ptr<Command>>();
		}

		void Undo()
		{
			if (m_UndoStack.empty())
			{
                OGN_CORE_WARN("No actions to undo");
                PUSH_CONSOLE_WARNING("No actions to undo");
				return;
			}

            auto command = std::move(m_UndoStack.top());
            m_UndoStack.pop();
            command->Undo();
            m_RedoStack.push(std::move(command));

            OGN_CORE_TRACE("Undo");
            PUSH_CONSOLE_INFO("Undo");
		}

		void Redo()
		{
			if (m_RedoStack.empty())
			{
                OGN_CORE_WARN("No actions to redo");
                PUSH_CONSOLE_WARNING("No actions to redo");
				return;
			}

            auto command = std::move(m_RedoStack.top());
            m_RedoStack.pop();
            command->Execute();
            m_UndoStack.push(std::move(command));

            OGN_CORE_TRACE("Redo");
            PUSH_CONSOLE_INFO("Redo");
		}

	private:
		std::stack<std::unique_ptr<Command>> m_UndoStack;
		std::stack<std::unique_ptr<Command>> m_RedoStack;
	};
}
