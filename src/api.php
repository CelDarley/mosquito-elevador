<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

Route::post('/enviar-mensagem', function (Request $request) {
    $mensagem = $request->input('deny');
    if ($mensagem == "0") {
        // Enviar mensagem para o ESP32
        $command = "/usr/bin/mosquitto_pub -h 10.100.0.104 -p 1883 -t esp32/acionar -m 'ON'";
        $output = shell_exec($command . " 2>&1");
        \Log::info('Comando MQTT executado:', [
            'comando' => $command,
            'saida' => $output,
            'usuario' => exec('whoami'),
            'path' => exec('echo $PATH')
        ]);
    } elseif ($mensagem == "1") {
        // Enviar mensagem para o ESP32
        $command = "/usr/bin/mosquitto_pub -h 10.100.0.104 -p 1883 -t esp32/acionar -m 'OFF'";
        $output = shell_exec($command . " 2>&1");
        \Log::info('Comando MQTT executado:', [
            'comando' => $command,
            'saida' => $output,
            'usuario' => exec('whoami'),
            'path' => exec('echo $PATH')
        ]);
    }
    return response()->json(['mensagem' => 'Mensagem enviada com sucesso!']);
});