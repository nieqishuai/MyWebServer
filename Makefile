# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LIBS = -lmysqlclient

# 版本信息
VERSION = 1.0.0
BUILD_DATE = $(shell date '+%Y-%m-%d %H:%M:%S')

# 编译模式 (DEBUG/RELEASE)
ifeq ($(DEBUG),1)
    CXXFLAGS += -g -O0 -DDEBUG
    BUILD_TYPE = debug
else
    CXXFLAGS += -O2 -DNDEBUG
    BUILD_TYPE = release
endif

# 目标文件
TARGET = bin/webserver
TARGET_DEBUG = bin/webserver_debug

# 源文件
SOURCES = code/main.cpp \
          code/buffer/buffer.cpp \
          code/http/httpconn.cpp \
          code/http/httprequest.cpp \
          code/http/httpresponse.cpp \
          code/log/log.cpp \
          code/pool/sqlconnpool.cpp \
          code/server/epoller.cpp \
          code/server/webserver.cpp \
          code/timer/heaptimer.cpp

# 头文件目录
INCLUDES = -Icode -Icode/buffer -Icode/http -Icode/log -Icode/pool -Icode/server -Icode/timer

# 默认目标
all: $(TARGET)

# 调试版本
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: $(TARGET_DEBUG)

# 发布版本
release: CXXFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# 编译目标
$(TARGET): $(SOURCES)
	@echo "编译发布版本..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo "编译完成: $(TARGET)"

$(TARGET_DEBUG): $(SOURCES)
	@echo "编译调试版本..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET_DEBUG) $(LIBS)
	@echo "编译完成: $(TARGET_DEBUG)"

# 运行服务器
run: $(TARGET)
	@echo "启动Web服务器..."
	./$(TARGET)

# 运行调试版本
run-debug: $(TARGET_DEBUG)
	@echo "启动调试版Web服务器..."
	./$(TARGET_DEBUG)

# 测试编译
test: $(TARGET)
	@echo "运行基本测试..."
	@if [ -f $(TARGET) ]; then \
		echo "✓ 可执行文件存在"; \
		file $(TARGET); \
		echo "✓ 编译成功"; \
	else \
		echo "✗ 编译失败"; \
		exit 1; \
	fi

# 安装到系统
install: $(TARGET)
	@echo "安装Web服务器到 /usr/local/bin/..."
	@sudo cp $(TARGET) /usr/local/bin/webserver
	@sudo chmod +x /usr/local/bin/webserver
	@echo "安装完成"

# 卸载
uninstall:
	@echo "卸载Web服务器..."
	@sudo rm -f /usr/local/bin/webserver
	@echo "卸载完成"

# 打包发布
package: release
	@echo "创建发布包..."
	@mkdir -p dist
	@cp $(TARGET) dist/
	@cp -r resources dist/ 2>/dev/null || true
	@cp README.md dist/ 2>/dev/null || true
	@tar -czf webserver-$(VERSION).tar.gz dist/
	@rm -rf dist/
	@echo "发布包创建完成: webserver-$(VERSION).tar.gz"

# 显示项目信息
info:
	@echo "=== Web服务器项目信息 ==="
	@echo "版本: $(VERSION)"
	@echo "构建时间: $(BUILD_DATE)"
	@echo "构建类型: $(BUILD_TYPE)"
	@echo "编译器: $(CXX)"
	@echo "编译选项: $(CXXFLAGS)"
	@echo "目标文件: $(TARGET)"
	@echo "源文件数量: $(words $(SOURCES))"
	@echo "=========================="

# 代码统计
stats:
	@echo "=== 代码统计 ==="
	@echo "源文件行数:"
	@wc -l $(SOURCES) | tail -1
	@echo "头文件:"
	@find code -name "*.h" | wc -l
	@echo "源文件:"
	@find code -name "*.cpp" | wc -l
	@echo "================"

# 清理
clean:
	@echo "清理编译产物..."
	rm -f $(TARGET) $(TARGET_DEBUG)
	rm -f *.tar.gz
	@echo "清理完成"

# 深度清理
distclean: clean
	@echo "深度清理..."
	rm -rf dist/
	find . -name "*.o" -delete
	find . -name "*.d" -delete
	@echo "深度清理完成"

# 重新编译
rebuild: clean all

# 帮助信息
help:
	@echo "可用的make目标:"
	@echo "  all        - 编译发布版本 (默认)"
	@echo "  debug      - 编译调试版本"
	@echo "  release    - 编译发布版本"
	@echo "  run        - 运行服务器"
	@echo "  run-debug  - 运行调试版本"
	@echo "  test       - 运行基本测试"
	@echo "  install    - 安装到系统"
	@echo "  uninstall  - 从系统卸载"
	@echo "  package    - 创建发布包"
	@echo "  info       - 显示项目信息"
	@echo "  stats      - 代码统计"
	@echo "  clean      - 清理编译产物"
	@echo "  distclean  - 深度清理"
	@echo "  rebuild    - 重新编译"
	@echo "  help       - 显示此帮助信息"
	@echo ""
	@echo "环境变量:"
	@echo "  DEBUG=1    - 启用调试模式"

.PHONY: all debug release run run-debug test install uninstall package info stats clean distclean rebuild help 