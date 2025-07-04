<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| API Routes
|--------------------------------------------------------------------------
|
| Here is where you can register API routes for your application. These
| routes are loaded by the RouteServiceProvider and all of them will
| be assigned to the "api" middleware group. Make something great!
|
*/

Route::post('enviar-mensagem', function (Request $request) {
    try {
        $mensagem = $request->input('deny');
        if ($mensagem == "0") {
            // Enviar mensagem para o ESP32 do elevador
            $command = "/usr/bin/mosquitto_pub -h mosquitto.catenasystem.com.br -p 8883 -t esp32/acionar -m 'OFF'";
            $output = shell_exec($command . " 2>&1");
            error_log('Comando MQTT executado: ' . $command . ' - Saída: ' . $output);
        } elseif ($mensagem == "1") {
            // Enviar mensagem para o ESP32 do elevador
            $command = "/usr/bin/mosquitto_pub -h mosquitto.catenasystem.com.br -p 8883 -t esp32/acionar -m 'ON'";
            $output = shell_exec($command . " 2>&1");
            error_log('Comando MQTT executado: ' . $command . ' - Saída: ' . $output);
        }
        return response()->json(['mensagem' => 'Mensagem enviada com sucesso!']);
    } catch (\Exception $e) {
        error_log('Erro ao executar comando MQTT: ' . $e->getMessage());
        return response()->json(['erro' => 'Erro ao executar comando'], 500);
    }
});

Route::post('deskbee-fch001', function (Request $request) {
    try {
        $mensagem = $request->input('deny');
        if ($mensagem == "0") {
            // Enviar mensagem para o dispositivo deskbee-fch001
            $command = "/usr/bin/mosquitto_pub -h mosquitto.catenasystem.com.br -p 8883 -t deskbee-fch001 -m 'OFF'";
            $output = shell_exec($command . " 2>&1");
            error_log('Comando MQTT executado: ' . $command . ' - Saída: ' . $output);
        } elseif ($mensagem == "1") {
            // Enviar mensagem para o dispositivo deskbee-fch001
            $command = "/usr/bin/mosquitto_pub -h mosquitto.catenasystem.com.br -p 8883 -t deskbee-fch001 -m 'ON'";
            $output = shell_exec($command . " 2>&1");
            error_log('Comando MQTT executado: ' . $command . ' - Saída: ' . $output);
        }
        return response()->json(['mensagem' => 'Mensagem enviada com sucesso!']);
    } catch (\Exception $e) {
        error_log('Erro ao executar comando MQTT: ' . $e->getMessage());
        return response()->json(['erro' => 'Erro ao executar comando'], 500);
    }
}); 