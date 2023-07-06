#include "cmd_processor.h"

#include <iostream>
#include <sstream>
#include <string>
#include <list>

#include "task_list.h"
#include "tools.h"

using namespace std;

bool CmdProcessor::GetArgv(int argc, char* argv[], int start) {
  if (start == argc) {
    return false;
  }

  for (int i = start; i < argc; i++) {
    cmd_.push_back(argv[i]);
  }
  return true;
}

bool CmdProcessor::GetCmd() {
  string tmp;
  getline(cin, tmp);
  istringstream cmd(tmp);

  cmd >> tmp;
  if (!cmd) return false;
  cmd_.push_back(tmp);
  while (cmd >> tmp) {
    cmd_.push_back(tmp);
  }
  return true;
}

int CmdProcessor::CmdDistributor(TaskList& task_list) const {
  const int kSize = cmd_.size();
  if (kSize < 1) return false;

  bool flag = false;
  string first_cmd = cmd_.front();
  to_lower(first_cmd);

  if (first_cmd == "login") {
    // TODO login
  } else if (first_cmd == "addtask" || first_cmd == "add") {
    if (kSize == 1)
      flag = AddTaskNoOp(task_list);
    else
      flag = AddTaskOp(task_list, cmd_);
  } else if (first_cmd == "modifytask" || first_cmd == "modify") {
    if (kSize == 1)
      flag = ModifyTaskNoOp(task_list);
    else
      flag = ModifyTaskOp(task_list, cmd_);
  } else if (first_cmd == "deletetask" || first_cmd == "delete") {
    if (kSize == 1)
      flag = DeleteTaskNoOp(task_list);
    else
      flag = DeleteTaskOp(task_list, cmd_);
  } else if (first_cmd == "showtask" || first_cmd == "show") {
    if (kSize == 1)
      flag = ShowTaskNoOp(task_list);
    else
      flag = ShowTaskOp(task_list, cmd_);
  } else if (first_cmd == "searchtask" || first_cmd == "search") {
    if (kSize == 1)
      flag = SearchTaskNoOp(task_list);
    else
      flag = SearchTaskOp(task_list, cmd_);
  } else if (first_cmd == "quit" || first_cmd == "q") {
    cout << "Bye.\n";
    return -1;
  } else {
    cout << "\"" << cmd_.front() << "\" is not a valid command.\n";
    return 0;
  }
  return flag;
}

// 用户输入格式:addtask -name n -begin b -priority p -type t -remind r (注意:用户输入的参数不能以-开头,priority默认为0,type默认为" ",选项的顺序可以打乱)
bool AddTaskOp(TaskList& task_list, list<string> cmd)
{
  Other task;
  task.priority = 0;
  task.type = " ";
  string begin;                              // begin用于保存begin_time_并分配唯一的id
  list<string>::iterator it = ++cmd.begin(); // 从cmd的第二个string开始参考
  while (it != cmd.end())
  {
    if (*it == "-name")
    {
      if (++it == cmd.end()) //-name是最后一个,后面没有参数
          return false;
      string str = *(it); // 找到-name的下一位
      if (str[0] == '-')  // 不符合
          return false;
      task.name = str;
      it++;
    }
    else if (*it == "-begin")
    {
      if (++it == cmd.end()) //-begin是最后一个,后面没有参数
          return false;
      begin = *(it);       // 找到-begin的下一位
      if (begin[0] == '-') // 不符合
          return false;
      if (!isValidDate(begin)) // 时间参数不符合要求
          return false;
      task.begin_time = to_time_t(begin);
      it++;
    }
    else if (*it == "-priority")
    {
      if (++it == cmd.end()) //-priority是最后一个,后面没有参数,这是可以的
          break;
      string str = *(it);                                                          // 找到-priority的下一位
      if (str == "-name" || str == "-begin" || str == "-type" || str == "-remind") //-priority后不跟参数,这是可以的
          continue;
      int p = stoi(str);
      if (p != 1 && p != 2 && p != 4)
          return false;
      task.priority = p;
      it++;
    }
    else if (*it == "-type")
    {
      if (++it == cmd.end()) //-type是最后一个,后面没有参数,这是可以的
          break;
      string str = *(it);                                                              // 找到-type的下一位
      if (str == "-name" || str == "-begin" || str == "-priority" || str == "-remind") //-type后不跟参数,这是可以的
          continue;
      if (str != "entertainment" && str != "sport" && str != "study" && str != "routine")
          return false;
      task.type = str;
      it++;
    }
    else if (*it == "-remind")
    {
      if (++it == cmd.end()) //-remind是最后一个,后面没有参数
          return false;
      string str = *(it); // 找到-remind的下一位
      if (str[0] == '-')  // 不符合
          return false;
      if (!isValidDate(str)) // 时间参数不符合要求
          return false;
      task.begin_time = to_time_t(str);
      it++;
    }
    else
    {
      cout << "Invalid command" << endl;
      return false;
    }
  }

  int id = to_time_t(begin); // 得到id后打包进行add
  if (!task_list.Add(make_pair(id, task)))
  {
    cout << "Faliure" << endl;
    return false;
  }

  return true;
}


bool AddTaskNoOp(TaskList& task_list){
    // 添加任务名字
    Other newtask;
    std::cout << "Please input the name of the task" << endl;
    std::cin >> newtask.name;
    // 添加任务的开始时间
    std::cout << "Please input the begin time of the task" << endl;
    std::cout << "Time format should be 2022/02/02/03:00:00" << endl;
    string begin_t;
    bool valid_input = false;
    while (!valid_input)//检查任务的开始时间是不是%Y/%M/%D/%h:%m:%s的形式
    {
        std::cin >> begin_t;
        if (isValidDate(begin_t))//如果是退出循环
            valid_input = true;
        else
            std::cout << "Invalid format. Please try again." << endl;//否则一直输入直到是为止
    }
    newtask.begin_time = to_time_t(begin_t);//将用户输入的形式转化成time_t格式保存
    // 添加任务的优先级
    std::cout << "Please input the priority of the task" << endl;
    std::cout << "1-low, 2-medium, 4-high" << endl;
    std::cin >> newtask.priority;
    // 检查优先级是不是符合1，2，4
    while (newtask.priority != 1 && newtask.priority != 2 && newtask.priority != 4)
    {
        std::cout << "Invalid priority. Please try again." << endl;
        std::cin >> newtask.priority;
    }
    // 添加任务的种类
    std::cout << "Please input the type of the task：entertainment  sport  study  routine" << endl;
    std::cin >> newtask.type;
    // 添加任务的提醒时间
    std::cout << "Please input the remind time of the task" << endl;
    std::cout << "Time format should be 2022/02/02/03:00:00" << endl;
    string remind_t;
    valid_input = false;
    //检查任务的开始时间是不是%Y/%M/%D/%h:%m:%s的形式
    while (!valid_input)
    {
        std::cin >> remind_t;
        if (isValidDate(remind_t))
            valid_input = true;
        else
            std::cout << "Invalid format. Please try again." << endl;
    }
    newtask.remind_time = to_time_t(remind_t);//将用户输入的形式转化成time_t格式保存
    //id和开始时间一样
    int id = to_time_t(begin_t)%1000;
    while(task_list.FindTask(id) == task_list.task_list_.end()){
        id++;
    }
    int res = task_list.Add(make_pair(id, newtask));
    while ( res != 0){//如果用户输入的开始时间或名字重复了，返回值不是0
        if(res==-1){
            std::cout << "The begin time repeats! Please enter another begin time: " << endl;
            bool valid_input = false;
            while (!valid_input)//检查任务的开始时间是不是%Y/%M/%D/%h:%m:%s的形式
            {
                std::cin >> begin_t;
                if (isValidDate(begin_t))//如果是退出循环
                    valid_input = true;
                else
                    std::cout << "Invalid format. Please try again." << endl;//否则一直输入直到是为止
            }
            newtask.begin_time = to_time_t(begin_t);//将用户输入的形式转化成time_t格式保存
            id = to_time_t(begin_t)%1000;
            while(task_list.FindTask(id) == task_list.task_list_.end()){
                id++;
            }
            res = task_list.Add(make_pair(id, newtask));
        }
        else if(res==1){
            std::cout << "The name repeats! Please enter another name: " << endl;
            std::cin >> newtask.name;
            res = task_list.Add(make_pair(id, newtask));
        }
    }
    return true;
}

bool DeleteTaskNoOp(TaskList& task_list){
  std::cout << "Delete tasks based on name or ID" << endl;
  std::cout << "1 represents deletion based on ID\nwhile the remaining represents deletion based on name" << endl;

  int choice;
  std::cin >> choice;
  while (!std::cin) // 检查输入是否有效
  {
    std::cin.clear(); // 清除输入流状态标志
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

    std::cout << "Invalid input. Please enter a valid ID: " << endl;
    std::cin >> choice;
  }
  if (choice == 1)
  {
    std::cout << "Please input the ID of the task to be deleted" << endl;
    long long id;
    std::cin >> id;
    while (!std::cin || !task_list.Erase(id)) // 检查输入是否有效以及id是否存在
    {	//在输入有效的情况下才会执行Erase并判断返回值
      if(!std::cin){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid ID: " << endl;
        std::cin >> id;
      }
      else if(!task_list.Erase(id)){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Can not find this id. Please enter a valid ID: " << endl;
        std::cin >> id;
      }
    }
  }
  else
  {
    std::cout << "Please input the name of the task to be deleted" << endl; // 根据名称删除任务
    string taskName;
    std::cin >> taskName;
    while (!task_list.Erase(taskName))
    {
      std::cin.clear(); // 清除输入流状态标志
      std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

      std::cout << "Can not find this name. Please enter a valid name: " << endl;
      std::cin >> taskName;
    }

  }
  task_list.saveFile();//保存文件
  return true;
}

bool ShowTaskNoOp(const TaskList& task_list){
  std::cout << "If you want to see all the tasks, input 0" << endl;
			std::cout << "If you need to select according to the importance of the task, input 1" << endl;
			std::cout << "If you need to select according to the time of the task, input 2" << endl;
			std::cout << "If you need to select according to both the time and importance of the task, input 3" << endl;
			std::cout << "default: show all" << endl;

			int option;
			std::cin >> option;
			while (!std::cin) // 检查输入是否有效
			{
				std::cin.clear(); // 清除输入流状态标志
				std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

				std::cout << "Invalid input. Please enter a valid choice: " << endl;
				std::cin >> option;
			}
			int priority = 7;
			int startTime = 0;
			string startTimeStr = "";
			string endTimeStr = "";
			string c;//由用户决定是否要设定时间
			int endTime = pow(2, 31) - 1;

			switch (option)
			{
			case 0:
				task_list.Show(); // 显示所有任务
				break;
			case 1:
				std::cout << "1 for low, 2 for medium, 4 for high" << endl;
				std::cout << "3 for low and medium, 6 for medium and high, 5 for low and high" << endl;
				std::cout << "7 for all" << endl;
				std::cin >> priority;
				while (priority > 7 || priority < 1)
				{
					std::cout << "invalid" << endl;
					std::cin >> priority;
				}
				while (!std::cin) // 检查输入是否有效
				{
					std::cin.clear(); // 清除输入流状态标志
					std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

					std::cout << "Invalid input. Please enter a valid choice: " << endl;
					std::cin >> priority;
				}
				task_list.Show(0, pow(2, 31) - 1, priority); // 根据优先级显示任务
				break;
			case 2:
				std::cout << "If you want to see all the tasks before the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y") {
					std::cout << "Please input the date" << endl;
					std::cin >> startTimeStr;

					while (!isValidDate(startTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> startTimeStr;
					}

				}

				if (startTimeStr != "")
					startTime = to_time_t(startTimeStr);

				std::cout << "If you want to see all the tasks after the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y")
				{
					std::cout << "Please input the date" << endl;
					std::cin >> endTimeStr;

					while (!isValidDate(endTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> endTimeStr;
					}

				}

				if (endTimeStr != "")
					endTime = to_time_t(endTimeStr);

				task_list.Show(startTime, endTime); // 根据时间范围显示任务
				break;
			case 3:
				std::cout << "1 for low, 2 for medium, 4 for high" << endl;
				std::cout << "3 for low and medium, 6 for medium and high, 5 for low and high" << endl;
				std::cout << "7 for all" << endl;
				std::cin >> priority;
				while (priority > 7 || priority < 1)
				{
					std::cout << "invalid" << endl;
					std::cin >> priority;
				}
				while (!std::cin) // 检查输入是否有效
				{
					std::cin.clear(); // 清除输入流状态标志
					std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

					std::cout << "Invalid input. Please enter a valid choice: " << endl;
					std::cin >> priority;
				}
				std::cout << "If you want to see all the tasks before the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y") {
					std::cout << "Please input the date" << endl;
					std::cin >> startTimeStr;

					while (!isValidDate(startTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> startTimeStr;
					}

				}

				if (startTimeStr != "")
					startTime = to_time_t(startTimeStr);

				std::cout << "If you want to see all the tasks after the specified date, enter y/n" << endl;
				std::cout << "format: 2022/02/02/10:00:00" << endl;
				std::cin >> c;

				if (c == "y" || c == "Y") {
					std::cout << "Please input the date" << endl;
					std::cin >> endTimeStr;

					while (!isValidDate(endTimeStr))
					{
						std::cout << "Invalid input, please try again" << endl;
						std::cin >> endTimeStr;
					}

				}

				if (endTimeStr != "")
					endTime = to_time_t(endTimeStr);

				task_list.Show(startTime, endTime, priority); // 根据时间范围和优先级
			}
}

bool ModifyTaskNoOp(TaskList& task_list){
  std::cout << "search tasks based on name or ID" << endl;
  std::cout << "1 represents search based on ID\nwhile the remaining represents search based on name" << endl;
  int choice;
  std::cin >> choice;
  while (!std::cin) // 检查输入是否有效
  {
    std::cin.clear(); // 清除输入流状态标志
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

    std::cout << "Invalid input. Please enter a valid choice: " << endl;
    std::cin >> choice;
  }
  vector<pair<int, Other>>::iterator it;
  string newstart;
  string newremind;
  switch (choice)
  {
  case 1:
    std::cout << "please input the id of the wanted task" << endl;
    long long id;
    std::cin >> id;
    while (!std::cin || !task_list.FindShow(id)) // 检查输入是否有效
    {
      if(!std::cin){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid id: " << endl;
        std::cin >> id;
      }
      else if(!task_list.FindShow(id)){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Can not find this id. Please enter a valid id: " << endl;
        std::cin >> id;
      }
    }
    
    it = task_list.FindTask(id);
    break;
  default:
    std::cout << "please input the name of the wanted task" << endl;
    string idx;
    std::cin >> idx;

    while (!task_list.FindShow(idx)) // 检查输入是否有效
    {
      if(!task_list.FindShow(idx)){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Can not find this name. Please enter a valid id: " << endl;
        std::cin >> idx;
      }
    }

    it = task_list.FindTask(idx);
  }
  std::cout << "n : change name" << endl;
  std::cout << "s : change start time" << endl;
  std::cout << "r : change remind time" << endl;
  std::cout << "t : change type " << endl;
  std::cout << "p : change priority " << endl;

  string xx;
  std::cin >> xx;
  bool is_invalid=true;
  while(is_invalid){
    if (xx == "n" || xx == "N") {
      std::cout << "change name" << endl;
      std::cin >> it->second.name;
      is_invalid=false;
    }
    else if (xx == "s" || xx == "S") {
      std::cout << "change begin_time" << endl;
      std::cin >> newstart;
      while(!isValidDate(newstart)){
        cout << "invalid date format" << endl;
        cin >> newstart;
      }
      if(it->second.begin_time != to_time_t(newstart))
        it->second.begin_time = to_time_t(newstart);
      is_invalid=false;
    }
    else if (xx == "t" || xx == "T") {
      std::cout << "change type" << endl;
      std::cin >> it->second.type;
      is_invalid=false;
    }
    else if (xx == "r" || xx == "R") {
      std::cout << "change remind_time" << endl;
      std::cin >> newremind;
      while(!isValidDate(newremind)){
        cout << "invalid date format" << endl;
        cin >> newremind;
      }
      if(it->second.remind_time != to_time_t(newremind))
        it->second.remind_time = to_time_t(newremind);
      is_invalid=false;
    }
    else if (xx == "p" || xx == "P") {
      std::cout << "change prority" << endl;
      std::cin >> it->second.priority;
      is_invalid=false;
    }
    else{
      std::cout << "Invalid choice. Please enter a valid choice: " << endl;
      std::cin >> xx;
    }
      
  }
  
  task_list.saveFile();
}

bool SearchTaskNoOp(TaskList& task_list){
  std::cout << "search tasks based on name or ID" << endl;
  std::cout << "1 represents search based on ID\nwhile the remaining represents search based on name" << endl;
  int choice;
  std::cin >> choice;
  while (!std::cin) // 检查输入是否有效
  {
    std::cin.clear(); // 清除输入流状态标志
    std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

    std::cout << "Invalid input. Please enter a valid choice: " << endl;
    std::cin >> choice;
  }
  switch (choice)
  {
  case 1:
    std::cout << "please input the id of the wanted task" << endl;
    long long id;
    std::cin >> id;
    while (!std::cin || !task_list.FindShow(id)) // 检查输入是否有效
    {
      if(!std::cin){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Invalid input. Please enter a valid id: " << endl;
        std::cin >> id;
      }
      else if(!task_list.FindShow(id)){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Can not find this id. Please enter a valid id: " << endl;
        std::cin >> id;
      }
    }
    break;
  default:
    std::cout << "please input the name of the wanted task" << endl;
    string ix;
    std::cin >> ix;
    while (!task_list.FindShow(ix)) // 检查输入是否有效
    {
      if(!task_list.FindShow(ix)){
        std::cin.clear(); // 清除输入流状态标志
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略剩余的输入

        std::cout << "Can not find this name. Please enter a valid id: " << endl;
        std::cin >> ix;
      }
    }
    task_list.FindShow(ix);
    break;
  }
}
