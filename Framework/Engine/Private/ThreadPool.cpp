#include "ThreadPool.h"

IMPLEMENT_SINGLETON(CThreadPool)

CThreadPool::CThreadPool()
{
}

CThreadPool::~CThreadPool()
{
}

HRESULT CThreadPool::Initialize(_uint _iThreadsNum)
{

    for (_uint i = 0; i < _iThreadsNum; ++i) {
        m_workers.emplace_back([&]() {
            /*작업이 추가되기까지 무한정 대기했다가 작업이 추가되면 수행한다*/
            while (true) {

                std::function<void()> task;
                {
                    /*임계구역에 다른 스레드가 접근하지 못하도록 lock을 건다 -> task queue에 접근*/
                    unique_lock<std::mutex> lock(m_mutex);
                    /*Stop이거나 empty가 아닐때까지 대기*/
                    m_condition.wait(lock,
                        [&] { return m_isStop || !m_tasks.empty(); });
                    if (m_isStop && m_tasks.empty())
                        return;
                    /*맨 앞에 있는 task를 pop해서 빼온다*/
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                /*task 수행*/
                task();
            }

            }
        );
    }

    return S_OK;
}

//template<class F, class ...Args>
//auto CThreadPool::ThreadEnqueue(F&& f, Args && ...args) -> std::future<typename std::invoke_result<F,Args ...>::type>
//{
//    using return_type = typename std::result_of<F(Args...)>::type;
//
//    /*thread가 처리해야 할 task*/
//    auto task = std::make_shared< std::packaged_task<return_type()> >(
//        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
//    );
//
//
//    std::future<return_type> res = task->get_future();
//    {
//        std::unique_lock<std::mutex> lock(m_mutex);
//
//        /*stop 일때는 추가적인 작업을 하면 안되기 때문에 예외처리*/
//        if (m_isStop)
//            throw std::runtime_error("enqueue on stopped ThreadPool");
//
//        m_tasks.emplace([task]() { (*task)(); });
//    }
//
//    m_condition.notify_one();
//
//    return res;
//}