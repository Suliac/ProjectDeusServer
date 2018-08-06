#pragma once
namespace DeusServer
{
	class IExecutable
	{
	public:
		void Update(double deltatimeMs);
		void Start();
		void Stop();

	protected:
		virtual void OnUpdate(double deltatimeMs) = 0;
		virtual void OnStart() = 0;
		virtual void OnStop() = 0;

		bool m_isStopped = true;
	};
}

