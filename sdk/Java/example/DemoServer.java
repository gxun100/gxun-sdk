package com.kd100.demo.example;

import com.gxun.core.*;
import java.util.Scanner;

public class DemoServer {
    private static final String WORKING_DIR = ".";

    private static final int GID = -1;
    private static final int SID = -1;
    private static final String LISTENING_IP = "0.0.0.0";
    private static final int LISTENING_PORT = 4444;

    public static void main(String[] args) throws GXunException {
        GXunNodeConfig config = GXunNodeConfig.newBuilder()
                //设置结点配置路径
                .setConfPath(WORKING_DIR + "/demo.conf")
                //若设置，使用这里的值为准；不设置则使用配置文件中的值
                //.setGid(GID)
                //.setSubId(SID)
                //.setListenIp(LISTENING_IP)
                .setListenPort(LISTENING_PORT)
                .build();
        //传入结点配置参数，启动结点实例
        GXunNode node = GXunNode.create(config);
        if (node == null) {
            throw new RuntimeException("FAIL TO CREATE NODE");
        }
        //设置Handler,Handler实现中处理到达消息
        node.setHandler(new ServerIOHandler());
        blockUntilInputExit();
        node.destroy();
        System.out.println("SERVER EXIT");
    }

    private static void blockUntilInputExit() {
        System.out.println("INPUT 'EXIT' TO RELEASE.");
        Scanner scanner = new Scanner(System.in);
        String line;
        do {
            line = scanner.nextLine();
        } while (!"exit".equalsIgnoreCase(line));
    }

    private static class ServerIOHandler extends GXunIOHandler {

        @Override
        protected void onDataArrive(GXunConnection connection, byte[] data) {
            System.out.println("RECEIVE DATA:" + data.length);
            byte[] response = new byte[9];
            System.arraycopy(data, 0, response, 0, 9);
            connection.send(response, GXunConnection.MsgAction.RESPONSE, false, 1000);
        }

        @Override
        protected boolean onNewConnection(GXunConnection connection) {
            System.out.println("ON NEW CONNECTION:" + connection.getConnId());
            return false;
        }

        @Override
        protected void onCloseConnection(GXunConnection connection) {
            System.out.println("ON CLOSE CONNECTION:" + connection.getConnId());
        }
    }
}
